#pragma once
#include "OSThread.h"

class CPUWorker : public OSThread
{
private:
public:
	void update(bool isRunning);

	void run() override;

}; 