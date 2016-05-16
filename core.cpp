#include <thread>
#include <functional>
#include <chrono>
#include "core.h"
#include "logger.h"

Core::Core(const AppConf & appConf):
    dBusHandler(appConf.GetDBusParams()),
    netConfReader(dBusHandler, appConf.GetDBusParams().period),
    fcgiHandler(netConfReader, appConf.GetFcgiParams()),
    fcgiThreadsNum(appConf.GetFcgiParams().threadsNum)
{
}

bool Core::Init()
{
    if(!fcgiHandler.Init())
    {
        Logger::instance() << "fcgiHandler initialization error!";
        return false;
    }
    if(!dBusHandler.Init())
    {
        Logger::instance() << "dBusHandler initialization error!";
        return false;
    }

    return true;
}

void Core::Work()
{
    std::vector<std::thread> threads;

    threads.push_back(std::thread(&NetConfReader::ThreadFunc, &netConfReader));

    for(int i=0; i<fcgiThreadsNum; i++)
    {
        threads.push_back(std::thread(&FcgiHandler::ThreadFunc, &fcgiHandler));
    }

    for(auto & thread: threads)
        thread.join();
}
