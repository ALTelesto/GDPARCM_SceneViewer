#pragma once
class IExecutionEvent
{
public:
	virtual void onFinishedExecution(int index) = 0;
};


