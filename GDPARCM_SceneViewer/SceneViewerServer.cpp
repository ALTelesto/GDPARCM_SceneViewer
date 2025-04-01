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
		progress = static_cast<float>(loaded / totalObj);

		if(progress >= 1.0f)
		{
			progress = 1.0f;
		}

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

grpc::Status SceneViewerServer::GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
                                         sceneviewer::Scene* response)
{
	if(SceneManager::getInstance()->getProgress(request->scene_id()) < 1.0f)
	{
		return grpc::Status(
			grpc::StatusCode::FAILED_PRECONDITION,
			"[SCENE REQUEST] Scene not fully loaded yet"
		);
	}

	const auto& models = SceneManager::getInstance()->getScene(request->scene_id());
	if (models.empty()) {
		return grpc::Status(
			grpc::StatusCode::NOT_FOUND,
			"[SCENE REQUEST] No models found in scene"
		);
	}

	response->mutable_models()->Reserve(models.size());

	for (const Model3D* model : models) {
		if (!model) continue;
		sceneviewer::Model* proto_model = response->add_models();
		ConvertModel3DToProto(*model, proto_model);
	}

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

Model3D* SceneViewerServer::loadObject(SceneID index)
{
	int objIndex = index;
	int obj2Index = index + 1;
	if (obj2Index > this->objPaths.size() - 1) obj2Index = 0;
	std::string path;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> iDistribution(0, 1);

	std::uniform_real_distribution<float> fDistribution(-this->positionMax,this->positionMax);

	int choice = iDistribution(generator);
	if(choice == 0)
	{
		path = this->objPaths[static_cast<int>(objIndex)];
	}
	else
	{
		path = this->objPaths[static_cast<int>(obj2Index)];
	}

	std::vector<GLuint> mesh_indices;
	glm::vec3 ObjectPos = glm::vec3(fDistribution(generator), fDistribution(generator), fDistribution(generator));

	std::vector<tinyobj::shape_t> shape;
	std::vector<tinyobj::material_t> material;
	std::string warning, error;

	tinyobj::attrib_t attributes;

	bool success = tinyobj::LoadObj(&attributes, &shape, &material, &warning, &error, path.c_str());

	if(!success)
	{
		std::cout << "[OBJ LOADING] ERROR model failed to load, path is: " + path << std::endl;
	}

	for (int i = 0; i < shape[0].mesh.indices.size(); i++) {
		mesh_indices.push_back(shape[0].mesh.indices[i].vertex_index);
	}

	//creation of model
	Model3D* newModel = new Model3D(ObjectPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(1, 1, 1), attributes.vertices, mesh_indices);

	std::uniform_int_distribution<> timeDistribution(500, 1500);

	std::this_thread::sleep_for(std::chrono::milliseconds(timeDistribution(generator)));

	return newModel;
}

void SceneViewerServer::loadObjFilePaths()
{
	std::cout << "[SceneViewerServer] Reading from asset list" << std::endl;
	std::ifstream stream("3D/models.txt");
	std::string path;

	while (std::getline(stream, path))
	{
		this->objPaths.push_back(path);
		std::cout << "[SceneViewerServer] Loaded model path: " << path << std::endl;
	}
}

void SceneViewerServer::run()
{
	RunServer(50051);
}
