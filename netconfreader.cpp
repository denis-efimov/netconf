#include "netconfreader.h"
#include <sstream>
#include <thread>

NetConfReader::NetConfReader()
{
}

NetConfReader::~NetConfReader()
{
}

boost::property_tree::ptree NetConfReader::GetNetConfiguration()
{
    std::lock_guard<std::mutex> lock(confPTreeMutex);
    return confPTree;
}

void NetConfReader::ThreadFunc(std::chrono::seconds period)
{
    for(;;)
    {
        UpdateNetConfiguration();
        std::this_thread::sleep_for(period);
    }
}

void NetConfReader::UpdateNetConfiguration()
{
    if(dBusHandler.Connect())
    {
        int deviceNum = 0;
        boost::property_tree::ptree newPT;
        for(auto & devicePath: GetDevicesPaths())
        {
            std::string configPath = GetConfigPaths(devicePath.c_str());
            std::pair<uint32_t, uint32_t> ipAndMask = GetIpAndMask(configPath.c_str());
            boost::property_tree::ptree innerPT;

            innerPT.put("ip", uintIpToString(ipAndMask.first));
            innerPT.put("mask", uintIpToString(ipAndMask.second));
            innerPT.put("gateway", GetGateway(configPath.c_str()));

            std::ostringstream deviceName;
            deviceName << "device" << deviceNum++;
            newPT.put_child(deviceName.str(), innerPT);
        }
        std::lock_guard<std::mutex> lock(confPTreeMutex);
        confPTree = newPT;
    }
}

std::string NetConfReader::GetConfigPaths(const char * path)
{
    std::string confPath;
    DBusPendingCall * pending;
    if(dBusHandler.PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.Device",
                  "Ip4Config",
                  pending))
    {
        DBusMessage* msg;
        if(dBusHandler.GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);

                confPath.append(dBusHandler.GetString(sub));
            }
        }
        dbus_message_unref(msg);
    }
    return confPath;
}

std::vector<std::string> NetConfReader::GetDevicesPaths()
{
    std::vector<std::string> devices;
    DBusPendingCall * pending;
    if(dBusHandler.MethodCall("org.freedesktop.NetworkManager",
                  "/org/freedesktop/NetworkManager",
                  "org.freedesktop.NetworkManager",
                  "GetDevices",
                  pending))
    {
        DBusMessage* msg;
        if(dBusHandler.GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);
                do
                {
                    devices.push_back(dBusHandler.GetString(sub));
                }while(dbus_message_iter_next(&sub));
            }
        }
        dbus_message_unref(msg);
    }
    return devices;
}

std::pair<uint32_t, uint32_t> NetConfReader::GetIpAndMask(const char * path)
{
    std::pair<uint32_t, uint32_t> ipAndMask;
    DBusPendingCall * pending;
    if(dBusHandler.PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.IP4Config",
                  "Addresses",
                  pending))
    {
        DBusMessage* msg;
        if(dBusHandler.GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);

                DBusMessageIter sub2;
                dbus_message_iter_recurse(&sub, &sub2);

                DBusMessageIter sub3;
                dbus_message_iter_recurse(&sub2, &sub3);

                uint32_t ip;
                dbus_message_iter_get_basic(&sub3, &ip);

                dbus_message_iter_next(&sub3);

                uint32_t maskBits;
                dbus_message_iter_get_basic(&sub3, &maskBits);

                uint32_t mask = 0;
                for(int i=0; i<maskBits; i++)
                    mask |= 0x80000000 >> i;
                mask = htobe32(mask);

                dbus_message_iter_next(&sub3);

                ipAndMask = std::make_pair(ip, mask);
            }
        }
        dbus_message_unref(msg);
    }
    return ipAndMask;
}

std::string NetConfReader::GetGateway(const char *path)
{
    std::string gateway;
    DBusPendingCall * pending;
    if(dBusHandler.PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.IP4Config",
                  "Gateway",
                  pending))
    {
        DBusMessage* msg;
        if(dBusHandler.GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);

                gateway = dBusHandler.GetString(sub);
            }
        }
        dbus_message_unref(msg);
    }
    return gateway;
}

std::string NetConfReader::uintIpToString(uint32_t ip)
{
    std::ostringstream ipStr;
    ipStr << ((ip >> 0) & 0xFF) << "."
          << ((ip >> 8) & 0xFF) << "."
          << ((ip >> 16) & 0xFF) << "."
          << ((ip >> 24) & 0xFF);
    return ipStr.str();
}
