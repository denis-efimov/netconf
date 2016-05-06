#ifndef NETCONFREADER_H
#define NETCONFREADER_H

#include <dbus/dbus.h>
#include <string>
#include <vector>

class NetConfReader
{
public:
    NetConfReader();
    ~NetConfReader();
    void GetNetConfiguration();

private:
    bool DBusConnect();
    bool MethodCall(const char * target, const char * object, const char * interface, const char * method, DBusPendingCall * & pending);
    bool PropertyRequest(const char * target, const char * object, const char * interface, const char * property, DBusPendingCall * & pending);
    bool GetReply(DBusPendingCall* & pending, DBusMessage* & msg);
    std::string DBusGetString(DBusMessageIter &iter);
    int DBusGetInt(DBusMessageIter & iter);
    std::vector<std::string> GetConfigPaths(const char * path);
    std::vector<std::string> GetDevicesPaths();
    std::vector<std::pair<uint32_t, uint32_t>> GetIpsAndMasks(const char * path);

private:
    DBusConnection* conn;
    bool isConnected;
};

#endif // NETCONFREADER_H
