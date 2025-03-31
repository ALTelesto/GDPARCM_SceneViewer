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
}
