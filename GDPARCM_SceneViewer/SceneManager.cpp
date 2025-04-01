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

SceneManager::SceneManager()
{
	for(int i = 0; i < 5; i++)
	{
		progress.push_back(0);
		loadedScenes.emplace_back();
		sceneActive.push_back(false);
		loadingGuard.push_back(new mutex());
	}
}

void SceneManager::setProgress(SceneID index, float value)
{
	this->progressGuard.lock();
	this->progress[index] = value;
	this->progressGuard.unlock();
}

float SceneManager::getProgress(SceneID index)
{
	return this->progress[index];
}

void SceneManager::loadScene(SceneID index, SceneLocal scene)
{
	this->sceneGuard.lock();
	for(Model3D* model : this->loadedScenes[index])
	{
		delete model;
	}
	this->loadedScenes[index].clear();
	for(Model3D* model : scene)
	{
		this->loadedScenes[index].push_back(model);
	}
	this->sceneGuard.unlock();
}

SceneLocal SceneManager::getScene(SceneID index)
{
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
	if(this->loadingGuard[index]->try_lock())
	{
		return true;
	}
	return false;
}

void SceneManager::reportDoneScene(SceneID index)
{
	this->loadingGuard[index]->unlock();
}

