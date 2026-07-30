#pragma once
class OSThread
{
protected:
	virtual void run() {}
public:
	OSThread();
	virtual ~OSThread();

	void start();
	static void sleep(int ms);
};

