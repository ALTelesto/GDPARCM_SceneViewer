#include "SceneViewerServer.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "SceneManager.h"
#include "Model3D.h"
#include "SceneViewerUtility.h"
#include "StringUtils.h"
#include "tiny_obj_loader.h"
#include "Model3D.h"

//grpc::server is multithreaded already apparently so we ditch these two
/*
grpc::Status SceneViewerServer::RequestLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
                                            sceneviewer::LoadResponse* response)
{
	//return Service::RequestLoad(context, request, response);
	if(!SceneManager::getInstance()->registerLoadScene(request->scene_id()))
	{
		return grpc::Status::CANCELLED;
	}
	return grpc::Status::OK;
}

grpc::Status SceneViewerServer::CheckProgress(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
	sceneviewer::ProgressResponse* response)
{
	response->set_progress(SceneManager::getInstance()->getProgress(request->scene_id()));
	response->set_ready(SceneManager::getInstance()->getProgress(request->scene_id()) >= 1.0f);
	return grpc::Status::OK;
}
*/

grpc::Status SceneViewerServer::StreamLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
	grpc::ServerWriter<sceneviewer::LoadProgress>* writer)
{
	LoadProgress response;

	response.set_load_accepted(true);
	if(!writer->Write(response))
	{
		return grpc::Status::CANCELLED;
	}

	if(!SceneManager::getInstance()->getScene(request->scene_id()).empty())
	{
		std::cout << "[SCENE LOADING] BYPASSING already loaded Scene " + to_string(request->scene_id()) << std::endl;
		response.set_progress(1);
		response.clear_load_accepted();
		response.set_ready(true);
		writer->Write(response);
		return grpc::Status::OK;
	}

	std::cout << "[SCENE LOADING] START loading Scene " + to_string(request->scene_id()) << std::endl;

	SceneLocal scene;
	float progress = 0.0f;
	int totalObj = this->objCount;
	int loaded = 0;
	while(progress < 1.0f)
	{
		if(context->IsCancelled())
		{
			std::cout << "[SCENE LOADING] FAILED loading Scene " + to_string(request->scene_id()) << std::endl;
			break;
		}

		Model3D* newModel = loadObject(request->scene_id());
		if(newModel != nullptr)
		{
			scene.push_back(newModel);
		}
		loaded++;
		progress = static_cast<float>(loaded) / totalObj;

		if(progress >= 1.0f)
		{
			progress = 1.0f;
		}

		std::cout << "[SCENE LOADING] Progress Scene " + to_string(request->scene_id()) + ": " + to_string(progress) << std::endl;
		response.clear_load_accepted();
		response.set_progress(progress);
		writer->Write(response);
	}

	SceneManager::getInstance()->loadScene(request->scene_id(), scene);

	if(!context->IsCancelled())
	{
		response.clear_progress();
		response.set_ready(true);
		writer->Write(response);
	}

	std::cout << "[SCENE LOADING] SUCCESS loading Scene " + to_string(request->scene_id()) << std::endl;
	return grpc::Status::OK;
}

grpc::Status SceneViewerServer::UnloadScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
	sceneviewer::UnloadResponse* response)
{
	std::cout << "[SCENE UNLOADING] START unloading Scene " + to_string(request->scene_id()) << std::endl;
	if(!SceneManager::getInstance()->unloadScene(request->scene_id()))
	{
		response->set_done(false);
		return grpc::Status::CANCELLED;
	}
	std::cout << "[SCENE UNLOADING] DONE unloading Scene " + to_string(request->scene_id()) << std::endl;
	response->set_done(true);
	return grpc::Status::OK;
}

grpc::Status SceneViewerServer::GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
                                         sceneviewer::Scene* response)
{
	if (SceneManager::getInstance()->getScene(request->scene_id()).empty()) {
		std::cout << "[SCENE GET] Early exit because not loaded Scene " + to_string(request->scene_id()) << std::endl;
		response->set_done(false);
		return grpc::Status::CANCELLED;
	}

	std::cout << "[SCENE GET] START getting Scene " + to_string(request->scene_id()) << std::endl;

	const auto& models = SceneManager::getInstance()->getScene(request->scene_id());
	response->set_done(false);

	std::vector<sceneviewer::Model> proto_models;
	proto_models.reserve(models.size());

	for (const Model3D* model : models) {
		if (!model) continue;

		sceneviewer::Model proto_model;
		ConvertModel3DToProto(*const_cast<Model3D*>(model), &proto_model);
		proto_models.push_back(proto_model);
	}

	std::cout << "[SCENE GET] START transcribing Scene " + to_string(request->scene_id()) << std::endl;

	response->mutable_models()->Reserve(proto_models.size());
	for (auto& proto_model : proto_models) {
		response->add_models()->Swap(&proto_model);
	}

	std::cout << "[SCENE GET] DONE getting Scene " + to_string(request->scene_id()) << std::endl;
	response->set_done(true);

	return grpc::Status::OK;
}

void SceneViewerServer::RunServer(uint16_t port)
{
	//initialize scenemanager
	SceneManager::getInstance();

	std::string serverAddress = absl::StrFormat("localhost:%d", port);
	SceneViewerServer service;

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	grpc::ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << serverAddress << std::endl;
	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

Model3D* SceneViewerServer::loadObject(SceneID index) {
	std::vector<std::string> objPaths = SceneManager::getInstance()->getObjFilePaths();

	if (objPaths.empty()) {
		std::cerr << "[OBJ LOADING] ERROR: No model paths available" << endl;
		return nullptr;
	}

	std::cerr << "[OBJ LOADING] Loading model" << endl;

	const size_t safe_idx1 = static_cast<size_t>(index) % objPaths.size();
	const size_t safe_idx2 = (safe_idx1 + 1) % objPaths.size();

	std::random_device rd;
	std::mt19937 gen(rd());
	const std::string& path = std::uniform_int_distribution<>(0, 1)(gen)
		? objPaths[safe_idx1]
		: objPaths[safe_idx2];

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	bool success = tinyobj::LoadObj(
		&attrib,
		&shapes,
		&materials,
		&warn,
		&err,
		path.c_str()
	);

	if (!warn.empty()) {
		std::cout << "[OBJ WARNING] " << warn << endl;
	}

	if (!err.empty() || !success) {
		std::cerr << "[OBJ ERROR] " << err << " in file: " << path << endl;
		return nullptr;
	}

	if (shapes.empty()) {
		std::cerr << "[OBJ ERROR] No shapes found in: " << path << endl;
		return nullptr;
	}

	std::vector<GLuint> indices;
	for (const auto& index : shapes[0].mesh.indices) {
		indices.push_back(index.vertex_index);
	}

	if (attrib.vertices.empty()) {
		std::cerr << "[OBJ ERROR] No vertices found in: " << path << endl;
		return nullptr;
	}

	std::cout << "indices size: " + to_string(indices.size()) + "\n";
	std::cout << "vertices size: " + to_string(attrib.vertices.size()) + "\n";

	std::uniform_real_distribution<float> dist(-positionMax, positionMax);
	std::uniform_real_distribution<float> scal(0.7, 1.0);
	glm::vec3 pos(dist(gen), dist(gen), dist(gen));

	std::this_thread::sleep_for(std::chrono::milliseconds(
		std::uniform_int_distribution<>(500, 3000)(gen)));

	return new Model3D(
		pos,
		glm::vec3(0.f),
		glm::vec3(scal(gen)),
		attrib.vertices,
		indices
	);
}

void SceneViewerServer::run()
{
	SceneManager::getInstance()->loadObjFilePaths();
	RunServer(50051);
}
