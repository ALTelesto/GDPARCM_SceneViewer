#pragma once
#include <cstdint>
#include "IETThread.h"
#include "sceneviewer.grpc.pb.h"
#include "sceneviewer.pb.h"

using namespace sceneviewer;
class SceneViewerServer final : public SceneViewerService::Service, public IETThread
{
public:
	/*grpc::Status RequestLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::LoadResponse* response) override;
	grpc::Status CheckProgress(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::ProgressResponse* response) override;*/
	grpc::Status StreamLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, grpc::ServerWriter<sceneviewer::LoadProgress>* writer) override;
	grpc::Status GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::Scene* response) override;
	static void RunServer(uint16_t port);

	void run() override;
private:

};

