#include "logger.h"

Logger & Logger::instance()
{
    static Logger lg;
    return lg;
}

Logger::Logger():
    logFile("log.txt")
{

}
