#include "SceneManager.h"
#include "Model3D.h"

SceneManager* SceneManager::sharedInstance = nullptr;

SceneManager* SceneManager::getInstance()
{
	if (sharedInstance == nullptr) {
		//initialize
		sharedInstance = new SceneManager();
	}

	return sharedInstance;
}

SceneManager::SceneManager() : loadingGuard(5)
{
	for(int i = 0; i < 5; i++)
	{
		SceneID index = static_cast<SceneID>(i);
		progress[index] = 0;
		loadedScenes.emplace_back();
		sceneActive.push_back(false);
	}
}

void SceneManager::setProgress(SceneID index, float value)
{
	std::lock_guard<std::mutex> lock(progressGuard);
	this->progress[index] = value;
}

float SceneManager::getProgress(SceneID index)
{
	return this->progress[index];
}

void SceneManager::loadScene(SceneID index, SceneLocal scene)
{
	std::cout << "[SceneManager] loading scene" << std::endl;
	this->sceneActive[index] = false;

	for(Model3D* model : this->loadedScenes[index])
	{
		std::lock_guard<std::mutex> lock(sceneGuard);
		delete model;
	}
	{
		std::lock_guard<std::mutex> lock(sceneGuard);
		this->loadedScenes[index].clear();
	}
	for(Model3D* model : scene)
	{
		std::lock_guard<std::mutex> lock(sceneGuard);
		this->loadedScenes[index].push_back(model);
	}
	std::cout << "[SceneManager] done loading scene" << std::endl;
}

bool SceneManager::unloadScene(SceneID index)
{
	if (this->loadedScenes[index].empty()) return false;

	std::cout << "[SceneManager] unloading scene" << std::endl;
	this->sceneActive[index] = false;

	{
		std::lock_guard<std::mutex> lock(progressGuard);
		this->progress[index] = 0;
	}

	for (Model3D* model : this->loadedScenes[index])
	{
		std::lock_guard<std::mutex> lock(sceneGuard);
		delete model;
	}
	{
		std::lock_guard<std::mutex> lock(sceneGuard);
		this->loadedScenes[index].clear();
	}

	std::cout << "[SceneManager] done unloading scene" << std::endl;
	return true;
}

SceneLocal SceneManager::getScene(SceneID index)
{
	std::lock_guard<std::mutex> lock(sceneGuard);
	return this->loadedScenes[index];
}

void SceneManager::setSceneActive(SceneID index, bool value)
{
	this->sceneActive[index] = value;
}

bool SceneManager::isSceneActive(SceneID index)
{
	return this->sceneActive[index];
}

bool SceneManager::registerLoadScene(SceneID index)
{
	if(this->loadingGuard[index].try_lock())
	{
		return true;
	}
	return false;
}

void SceneManager::reportDoneScene(SceneID index)
{
	this->loadingGuard[index].unlock();
}

void SceneManager::loadObjFilePaths()
{
	std::cout << "[SceneManager] Reading from asset list" << std::endl;
	std::ifstream stream("3D/models.txt");
	std::string path;

	while (std::getline(stream, path))
	{
		this->objPaths.push_back(path);
		std::cout << "[SceneManager] Loaded model path: " << path << std::endl;
	}
}

vector<string> SceneManager::getObjFilePaths()
{
	return this->objPaths;
}

