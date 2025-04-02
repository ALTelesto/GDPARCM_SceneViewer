#pragma once
#include <cstdint>
#include "IETThread.h"
#include "sceneviewer.grpc.pb.h"
#include "sceneviewer.pb.h"

using namespace sceneviewer;
using namespace std;

class Model3D;

class SceneViewerServer final : public SceneViewerService::Service, public IETThread
{
public:
	/*grpc::Status RequestLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::LoadResponse* response) override;
	grpc::Status CheckProgress(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::ProgressResponse* response) override;*/
	grpc::Status StreamLoad(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, grpc::ServerWriter<sceneviewer::LoadProgress>* writer) override;
	grpc::Status UnloadScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::UnloadResponse* response) override;
	grpc::Status GetScene(grpc::ServerContext* context, const sceneviewer::SceneRequest* request, sceneviewer::Scene* response) override;
	static void RunServer(uint16_t port);

	Model3D* loadObject(SceneID index);

	void run() override;
private:
	int objCount = 5;
	float positionMax = 7.f;
};

