#pragma once
#include <mutex>
#include "ThreadPool.h"
class IWorkerAction;

class ThreadManager
{
public:
	static ThreadManager* getInstance();

	void scheduleTask(IWorkerAction* action);
	std::mutex* getMutex();

private:
	ThreadManager();
	ThreadManager(ThreadManager const&) {};             // copy constructor is private
	ThreadManager& operator=(ThreadManager const&) {};  // assignment operator is private
	static ThreadManager* sharedInstance;

	ThreadPool* threadPool;
	std::mutex* guard;
};

