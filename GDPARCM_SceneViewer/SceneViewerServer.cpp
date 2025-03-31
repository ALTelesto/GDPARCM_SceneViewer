#include "SceneViewerServer.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

grpc::Status SceneViewerServer::GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request,
                                         sceneviewer::Scene* response)
{
	//return Service::GetScene(context, request, response);
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
