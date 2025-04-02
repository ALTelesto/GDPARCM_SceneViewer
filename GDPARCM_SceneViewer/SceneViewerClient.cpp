#include "SceneViewerClient.h"

#include "IExecutionEvent.h"
#include "SceneViewerUtility.h"
#include "Model3D.h"

SceneViewerClient::SceneViewerClient(sceneviewer::SceneID index, EClientAction action, std::shared_ptr<grpc::ChannelInterface> channel) : index(index), action(action)
{
	this->stub_ = SceneViewerService::NewStub(channel);
}

template<typename Func, typename... Args>
auto SceneViewerClient::ExecuteWithRetry(Func&& operation, int max_retries, int initial_delay_ms, Args&&... args)
-> decltype((this->*operation)(std::forward<Args>(args)...))
{
	int attempt = 0;
	while (attempt < max_retries) {
		try {
			return (this->*operation)(std::forward<Args>(args)...);
		}
		catch (const std::exception& e) {
			if (++attempt == max_retries) throw;
			int delay_ms = initial_delay_ms * (1 << attempt);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
		}
	}
	throw std::runtime_error("[SceneViewerClient "+to_string(this->index)+"] Max retries exceeded");
}

bool SceneViewerClient::StreamLoad(int max_retries, int initial_delay_ms)
{
	return ExecuteWithRetry(&SceneViewerClient::TryStreamLoad, max_retries, initial_delay_ms);
}

bool SceneViewerClient::UnloadScene(int max_retries, int initial_delay_ms)
{
	return ExecuteWithRetry(&SceneViewerClient::TryUnloadScene, max_retries, initial_delay_ms);
}

SceneLocal SceneViewerClient::GetScene(int max_retries, int initial_delay_ms)
{
    return ExecuteWithRetry(&SceneViewerClient::TryGetScene, max_retries, initial_delay_ms);
}

void SceneViewerClient::setExecutionEvent(IExecutionEvent* event)
{
    this->event = event;
}

bool SceneViewerClient::TryStreamLoad() {
    SceneRequest request;
    request.set_scene_id(index);
    
    ClientContext context;
    std::unique_ptr<grpc::ClientReader<LoadProgress>> reader(
        stub_->StreamLoad(&context, request)
    );
    
    LoadProgress progress;
    bool loadAccepted = false;
    bool loadComplete = false;

    std::cout << "[SceneViewerClient " + to_string(this->index) + "] streaming loading scene..." << std::endl;
    while (reader->Read(&progress)) {
        if (progress.has_load_accepted()) {
            loadAccepted = progress.load_accepted();
            if (!loadAccepted) break;
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] stream load accepted" << std::endl;
        }
        else if (!progress.ready()) {
            //std::cout << "[SceneViewerClient " + to_string(this->index) + "] set progress" << std::endl;
            SceneManager::getInstance()->setProgress(index, progress.progress());
        }
        else if (progress.ready()) {
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] ready" << std::endl;
            loadComplete = true;
            break;
        }
    }
    
    Status status = reader->Finish();
    return loadAccepted && loadComplete && status.ok();
}

bool SceneViewerClient::TryUnloadScene() {
    SceneRequest request;
    request.set_scene_id(index);
    
    ClientContext context;
    UnloadResponse response;
    Status status = stub_->UnloadScene(&context, request, &response);
    return status.ok() && response.done();
}

SceneLocal SceneViewerClient::TryGetScene() {
    const int max_attempts = 10;
    const int base_delay_ms = 200;

    std::cout << "[SceneViewerClient " + to_string(this->index) + "] Start getting scene" << std::endl;

    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        SceneRequest request;
        request.set_scene_id(index);

        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() +
            std::chrono::seconds(3));

        Scene scene;
        std::cout << "[SceneViewerClient " + to_string(this->index) + "] Made request to get scene" << std::endl;
        Status status = stub_->GetScene(&context, request, &scene);

        if (!status.ok()) {
            throw std::runtime_error("RPC failed: " + status.error_message());
        }

        if (scene.done()) {
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] Got DONE response" << std::endl;
            SceneLocal loadedModels;
            try {
                for (const auto& proto_model : scene.models()) {
                    if (auto model = ConvertProtoToModel3D(proto_model)) {
                        loadedModels.push_back(model);
                    }
                }
                return loadedModels;
            }
            catch (...) {
                for (auto model : loadedModels) delete model;
                throw;
            }
        }

        int delay_ms = base_delay_ms * (1 << std::min(attempt, 4));
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    throw std::runtime_error("Scene conversion not complete after " +
        std::to_string(max_attempts) + " attempts");
}

void SceneViewerClient::LoadAction()
{
    std::cout << "[SceneViewerClient " + to_string(this->index) + "] try scene streaming" << std::endl;
	if (StreamLoad()) {
		SceneLocal loadedScene = GetScene();
		if (!loadedScene.empty()) {
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] loading scene" << std::endl;
			SceneManager::getInstance()->loadScene(index, loadedScene);
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] activating scene" << std::endl;
			SceneManager::getInstance()->setSceneActive(index, true);
		}
		else
		{
            std::cout << "[SceneViewerClient " + to_string(this->index) + "] scene is empty!" << std::endl;
			SceneManager::getInstance()->setProgress(index, 0.0f);
            SceneManager::getInstance()->setSceneActive(index, false);
		}
	}
	else {
		SceneManager::getInstance()->setProgress(index, 0.0f);
	}
}

void SceneViewerClient::UnloadAction()
{
    std::cout << "[SceneViewerClient " + to_string(this->index) + "] try scene unload" << std::endl;
	if (UnloadScene()) {
        std::cout << "[SceneViewerClient " + to_string(this->index) + "] unloading scene" << std::endl;
		SceneManager::getInstance()->unloadScene(index);
	}
}

void SceneViewerClient::run()
{
    std::cout << "[SceneViewerClient " + to_string(this->index) + "] run" << std::endl;
    try {
        switch (action) {
        case LOAD: LoadAction(); break;
        case UNLOAD: UnloadAction(); break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[SceneViewerClient " + to_string(this->index) + "] Operation failed: " << e.what() << std::endl;
    }
    SceneManager::getInstance()->setSceneActive(index, true);
    if (event) event->onFinishedExecution(this->index);
    delete this;
}
