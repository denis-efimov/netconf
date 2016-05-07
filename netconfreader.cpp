#include "netconfreader.h"
#include <sstream>
#include <thread>

NetConfReader::NetConfReader():
    isConnected(false)
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
    if(DBusConnect())
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

bool NetConfReader::DBusConnect()
{
    if(isConnected)
        return true;

    DBusError err;
    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err))
    {
        //Connection Error
        dbus_error_free(&err);
        return false;
    }

    dbus_bus_request_name(conn, "test.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err))
    {
        //Name Error
        dbus_error_free(&err);
        return false;
    }

    isConnected = true;
    return true;
}

bool NetConfReader::MethodCall(const char *target, const char *object, const char *interface, const char *method, DBusPendingCall* & pending)
{
    DBusMessage* msg;

    msg = dbus_message_new_method_call(target, object, interface, method);
    if (NULL == msg)
        return false;
    if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) // -1 is default timeout
        return false;
    if (NULL == pending)
        return false;
    dbus_connection_flush(conn);

    dbus_message_unref(msg);

    return true;
}

bool NetConfReader::PropertyRequest(const char * target, const char * object, const char * interface, const char * property, DBusPendingCall* & pending)
{
    DBusMessage* msg;
    DBusMessageIter args;

    msg = dbus_message_new_method_call(target, object, "org.freedesktop.DBus.Properties", "Get");
    if (NULL == msg)
       return false;

    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface))
       return false;
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &property))
       return false;

    if (!dbus_connection_send_with_reply (conn, msg, &pending, -1))
       return false;
    if (NULL == pending)
       return false;

    dbus_connection_flush(conn);

    dbus_message_unref(msg);

    return true;
}

bool NetConfReader::GetReply(DBusPendingCall* & pending, DBusMessage* & msg)
{
    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (NULL == msg)
        return false;

    dbus_pending_call_unref(pending);

    return true;
}

std::string NetConfReader::DBusGetString(DBusMessageIter & iter)
{
    char * cstr;
    dbus_message_iter_get_basic(&iter, &cstr);
    return std::string(cstr);
}

int NetConfReader::DBusGetInt(DBusMessageIter &iter)
{
    int num;
    dbus_message_iter_get_basic(&iter, &num);
    return num;
}

std::string NetConfReader::GetConfigPaths(const char * path)
{
    std::string confPath;
    DBusPendingCall * pending;
    if(PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.Device",
                  "Ip4Config",
                  pending))
    {
        DBusMessage* msg;
        if(GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);

                confPath.append(DBusGetString(sub));
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
    if(MethodCall("org.freedesktop.NetworkManager",
                  "/org/freedesktop/NetworkManager",
                  "org.freedesktop.NetworkManager",
                  "GetDevices",
                  pending))
    {
        DBusMessage* msg;
        if(GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);
                do
                {
                    devices.push_back(DBusGetString(sub));
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
    if(PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.IP4Config",
                  "Addresses",
                  pending))
    {
        DBusMessage* msg;
        if(GetReply(pending, msg))
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
    if(PropertyRequest("org.freedesktop.NetworkManager",
                  path,
                  "org.freedesktop.NetworkManager.IP4Config",
                  "Gateway",
                  pending))
    {
        DBusMessage* msg;
        if(GetReply(pending, msg))
        {
            DBusMessageIter args;
            if(dbus_message_iter_init(msg, &args))
            {
                DBusMessageIter sub;
                dbus_message_iter_recurse(&args, &sub);

                gateway = DBusGetString(sub);
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
