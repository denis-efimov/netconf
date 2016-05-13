#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>

class Logger
{
public:
    static Logger & instance();

    template<class T>
    Logger & operator<<(T msg);

private:
    Logger();
    Logger(const Logger &);
    const Logger & operator=(const Logger &);

private:
    std::ofstream logFile;
    std::mutex logFileMutex;
};

template<class T>
Logger & Logger::operator<<(T msg)
{
    {
        std::lock_guard<std::mutex> lock(logFileMutex);
        logFile << msg;
        logFile.flush();
    }
    return instance();
}

#endif // LOGGER_H
