#ifndef NETCONFREADER_H
#define NETCONFREADER_H

#include <dbus/dbus.h>
#include <string>

class NetConfReader
{
public:
    NetConfReader();
    ~NetConfReader();
    void GetNetConfiguration();

private:
    bool DBusConnect();
    bool MethodCall(std::string target, std::string object, std::string interface, std::string method, DBusPendingCall* & pending);
    bool GetReply(DBusPendingCall* & pending, DBusMessage* & msg);

private:
    DBusConnection* conn;
    bool isConnected;
};

#endif // NETCONFREADER_H
