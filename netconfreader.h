#ifndef NETCONFREADER_H
#define NETCONFREADER_H

#include <dbus/dbus.h>
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
    bool DBusConnect();
    bool MethodCall(const char * target, const char * object, const char * interface, const char * method, DBusPendingCall * & pending);
    bool PropertyRequest(const char * target, const char * object, const char * interface, const char * property, DBusPendingCall * & pending);
    bool GetReply(DBusPendingCall* & pending, DBusMessage* & msg);
    std::string DBusGetString(DBusMessageIter &iter);
    int DBusGetInt(DBusMessageIter & iter);
    std::__cxx11::string GetConfigPaths(const char * path);
    std::vector<std::string> GetDevicesPaths();
    std::pair<uint32_t, uint32_t> GetIpAndMask(const char * path);
    std::string GetGateway(const char * path);
    std::string uintIpToString(uint32_t ip);

private:
    DBusConnection* conn;
    boost::property_tree::ptree confPTree;
    std::mutex confPTreeMutex;
    bool isConnected;
};

#endif // NETCONFREADER_H
