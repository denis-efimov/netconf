#ifndef NETCONFREADER_H
#define NETCONFREADER_H

#include "dbushandler.h"
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <mutex>
#include <chrono>

class NetConfReader
{
public:
    NetConfReader();
    ~NetConfReader();
    boost::property_tree::ptree GetNetConfiguration();
    void ThreadFunc(std::chrono::seconds period);

private:
    void UpdateNetConfiguration();
    std::string GetConfigPaths(const char * path);
    std::vector<std::string> GetDevicesPaths();
    std::pair<uint32_t, uint32_t> GetIpAndMask(const char * path);
    std::string GetGateway(const char * path);
    std::string uintIpToString(uint32_t ip);

private:
    DBusHandler dBusHandler;
    boost::property_tree::ptree confPTree;
    std::mutex confPTreeMutex;
};

#endif // NETCONFREADER_H
