// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: sceneviewer.proto

#include "sceneviewer.pb.h"
#include "sceneviewer.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace sceneviewer {

static const char* SceneViewerService_method_names[] = {
  "/sceneviewer.SceneViewerService/StreamLoad",
  "/sceneviewer.SceneViewerService/GetScene",
};

std::unique_ptr< SceneViewerService::Stub> SceneViewerService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< SceneViewerService::Stub> stub(new SceneViewerService::Stub(channel, options));
  return stub;
}

SceneViewerService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_StreamLoad_(SceneViewerService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_GetScene_(SceneViewerService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::ClientReader< ::sceneviewer::LoadProgress>* SceneViewerService::Stub::StreamLoadRaw(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request) {
  return ::grpc::internal::ClientReaderFactory< ::sceneviewer::LoadProgress>::Create(channel_.get(), rpcmethod_StreamLoad_, context, request);
}

void SceneViewerService::Stub::async::StreamLoad(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest* request, ::grpc::ClientReadReactor< ::sceneviewer::LoadProgress>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::sceneviewer::LoadProgress>::Create(stub_->channel_.get(), stub_->rpcmethod_StreamLoad_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::sceneviewer::LoadProgress>* SceneViewerService::Stub::AsyncStreamLoadRaw(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::sceneviewer::LoadProgress>::Create(channel_.get(), cq, rpcmethod_StreamLoad_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::sceneviewer::LoadProgress>* SceneViewerService::Stub::PrepareAsyncStreamLoadRaw(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::sceneviewer::LoadProgress>::Create(channel_.get(), cq, rpcmethod_StreamLoad_, context, request, false, nullptr);
}

::grpc::Status SceneViewerService::Stub::GetScene(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request, ::sceneviewer::Scene* response) {
  return ::grpc::internal::BlockingUnaryCall< ::sceneviewer::SceneRequest, ::sceneviewer::Scene, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetScene_, context, request, response);
}

void SceneViewerService::Stub::async::GetScene(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest* request, ::sceneviewer::Scene* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::sceneviewer::SceneRequest, ::sceneviewer::Scene, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetScene_, context, request, response, std::move(f));
}

void SceneViewerService::Stub::async::GetScene(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest* request, ::sceneviewer::Scene* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetScene_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::sceneviewer::Scene>* SceneViewerService::Stub::PrepareAsyncGetSceneRaw(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::sceneviewer::Scene, ::sceneviewer::SceneRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetScene_, context, request);
}

::grpc::ClientAsyncResponseReader< ::sceneviewer::Scene>* SceneViewerService::Stub::AsyncGetSceneRaw(::grpc::ClientContext* context, const ::sceneviewer::SceneRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetSceneRaw(context, request, cq);
  result->StartCall();
  return result;
}

SceneViewerService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SceneViewerService_method_names[0],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< SceneViewerService::Service, ::sceneviewer::SceneRequest, ::sceneviewer::LoadProgress>(
          [](SceneViewerService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::sceneviewer::SceneRequest* req,
             ::grpc::ServerWriter<::sceneviewer::LoadProgress>* writer) {
               return service->StreamLoad(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SceneViewerService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SceneViewerService::Service, ::sceneviewer::SceneRequest, ::sceneviewer::Scene, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SceneViewerService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::sceneviewer::SceneRequest* req,
             ::sceneviewer::Scene* resp) {
               return service->GetScene(ctx, req, resp);
             }, this)));
}

SceneViewerService::Service::~Service() {
}

::grpc::Status SceneViewerService::Service::StreamLoad(::grpc::ServerContext* context, const ::sceneviewer::SceneRequest* request, ::grpc::ServerWriter< ::sceneviewer::LoadProgress>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SceneViewerService::Service::GetScene(::grpc::ServerContext* context, const ::sceneviewer::SceneRequest* request, ::sceneviewer::Scene* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace sceneviewer

