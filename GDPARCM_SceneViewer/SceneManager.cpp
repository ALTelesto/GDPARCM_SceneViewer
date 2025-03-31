#include "SceneManager.h"

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
	progress = { 0,0,0,0,0 };
	loadedScenes = { {},{},{},{},{} };
	sceneActive = { true,false,false,false,false };
}


void SceneManager::setProgress(int index, float value)
{
	this->progress[index] = value;
}

float SceneManager::getProgress(int index)
{
	return this->progress[index];
}

void SceneManager::loadScene(int index, SceneLocal scene)
{
	/*for(Model3D* model : this->loadedScenes[index])
	{
		delete model;
	}*/
	this->loadedScenes[index].clear();
	for(Model3D* model : scene)
	{
		this->loadedScenes[index].push_back(model);
	}
}

SceneLocal SceneManager::getScene(int index)
{
	return this->loadedScenes[index];
}

void SceneManager::setSceneActive(int index, bool value)
{
	this->sceneActive[index] = value;
}

bool SceneManager::isSceneActive(int index)
{
	return this->sceneActive[index];
}

