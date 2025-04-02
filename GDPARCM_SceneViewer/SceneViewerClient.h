#pragma once

#include "IETThread.h"
#include "SceneManager.h"
#include "sceneviewer.grpc.pb.h"

class IExecutionEvent;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

enum EClientAction
{
	LOAD,
	UNLOAD
};

class SceneViewerClient : public IETThread
{
public:
	SceneViewerClient(sceneviewer::SceneID index, EClientAction action,std::shared_ptr<grpc::ChannelInterface> channel);
	bool StreamLoad(int max_retries = 3, int initial_delay_ms = 100);
	bool UnloadScene(int max_retries = 3, int initial_delay_ms = 100);
	SceneLocal GetScene(int max_retries = 3, int initial_delay_ms = 100);

	void setExecutionEvent(IExecutionEvent* event);

protected:
	void run() override;

private:
	bool TryStreamLoad();
	bool TryUnloadScene();
	SceneLocal TryGetScene();

	template<typename Func, typename... Args>
	auto ExecuteWithRetry(Func&& operation, int max_retries, int initial_delay_ms, Args&&... args)
	-> decltype((this->*operation)(std::forward<Args>(args)...));

	void LoadAction();
	void UnloadAction();

	EClientAction action;
	sceneviewer::SceneID index;

	std::unique_ptr<SceneViewerService::Stub> stub_;

	IExecutionEvent* event;
};

