#include <thread>
#include <chrono>
#include "OSThread.h"

OSThread::OSThread() {}

OSThread::~OSThread() {}

void OSThread::start()
{
	//uncomment once AScheduler and CPUWorker are implemented
    //std::thread(&OSThread::run, this).detach();
}

void OSThread::sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
