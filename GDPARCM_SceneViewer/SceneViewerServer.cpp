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

	float progress = 0.0f;
	int totalObj = 2;
	int loaded = 0;
	while(progress < 1.0f)
	{
		if(context->IsCancelled())
		{
			break;
		}

		progress = static_cast<float>(loaded / totalObj);

		if(progress >= 1.0f)
		{
			progress = 1.0f;
		}

		response.clear_load_accepted();
		response.set_progress(progress);
		writer->Write(response);
	}

	if(!context->IsCancelled())
	{
		response.clear_progress();
		response.set_ready(true);
		writer->Write(response);
	}

	return grpc::Status::OK;
}

grpc::Status SceneViewerServer::GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
                                         sceneviewer::Scene* response)
{
	if(SceneManager::getInstance()->getProgress(request->scene_id()) < 1.0f)
	{
		return grpc::Status::CANCELLED;
	}
	
	return grpc::Status::OK;
}

void SceneViewerServer::RunServer(uint16_t port)
{
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

void SceneViewerServer::run()
{
	RunServer(50051);
}
