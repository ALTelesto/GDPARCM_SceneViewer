#include "ThreadManager.h"

ThreadManager* ThreadManager::sharedInstance = NULL;

ThreadManager* ThreadManager::getInstance()
{
	if (sharedInstance == NULL) {
		//initialize
		sharedInstance = new ThreadManager();
	}

	return sharedInstance;
}

ThreadManager::ThreadManager()
{
	this->guard = new std::mutex();
	this->threadPool = new ThreadPool("Thread Manager Threads", 12);
	this->threadPool->startScheduler();
}

void ThreadManager::scheduleTask(IWorkerAction* action)
{
	this->threadPool->scheduleTask(action);
}

std::mutex* ThreadManager::getMutex()
{
	return this->guard;
}


