#pragma once
#include <iostream>
#include <thread>
class IETThread
{
public:
	IETThread();
	virtual ~IETThread();

	void start();
	void dismiss();
	static void sleep(int ms);
protected:
	virtual void run() = 0;
	bool running = false;
private:
	
};

