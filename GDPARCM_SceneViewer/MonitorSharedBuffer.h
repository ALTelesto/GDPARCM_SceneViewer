#pragma once
#include <condition_variable>

class MonitorSharedBuffer
{
public:
	MonitorSharedBuffer();
	~MonitorSharedBuffer();

	void depositAmount(int amount);
	void withdrawAmount(int amount);
private:
	typedef std::condition_variable Condition;
	typedef std::mutex Mutex;
	typedef std::unique_lock<std::mutex> UniqueLock;

	Condition* conditionA;
	Mutex* guard;

	int totalAmount = 0;
	bool transactionDone = false;

	void tryWithdraw(int amount);
	void tryDeposit(int amount);
	void reportTransactionDone();
};

