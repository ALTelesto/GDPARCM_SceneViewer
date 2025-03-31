#include "MonitorSharedBuffer.h"

MonitorSharedBuffer::MonitorSharedBuffer()
{
	this->totalAmount = 0;
	this->guard = new Mutex();
	this->conditionA = new Condition();
}

MonitorSharedBuffer::~MonitorSharedBuffer()
{
	delete this->guard;
	delete this->conditionA;
}

void MonitorSharedBuffer::depositAmount(int amount)
{
	this->tryDeposit(amount);
	this->reportTransactionDone();
}

void MonitorSharedBuffer::withdrawAmount(int amount)
{
	this->tryWithdraw(amount);
	this->reportTransactionDone();
}

void MonitorSharedBuffer::tryWithdraw(int amount)
{
	UniqueLock uniqueLock(*this->guard);
	while(this->transactionDone == false)
	{
		this->conditionA->wait(uniqueLock);
	}

	this->totalAmount -= amount;
	uniqueLock.unlock();
}

void MonitorSharedBuffer::tryDeposit(int amount)
{
	UniqueLock uniqueLock(*this->guard);
	this->totalAmount += amount;
	uniqueLock.unlock();
}

void MonitorSharedBuffer::reportTransactionDone()
{
	UniqueLock uniqueLock(*this->guard);
	this->transactionDone = true;
	this->conditionA->notify_one();
	uniqueLock.unlock();
}
