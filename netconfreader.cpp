#include "netconfreader.h"

NetConfReader::NetConfReader():
    isConnected(false)
{
}

NetConfReader::~NetConfReader()
{
}

void NetConfReader::GetNetConfiguration()
{
    DBusPendingCall* pending;
    DBusMessage* msg;
    if(DBusConnect())
    {
        if(MethodCall("org.freedesktop.NetworkManager",
                      "/org/freedesktop/NetworkManager",
                      "org.freedesktop.NetworkManager",
                      "GetDevices",
                      pending))
        {
            if(GetReply(pending, msg))
            {
                //TODO
                dbus_message_unref(msg);
            }
        }
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

bool NetConfReader::MethodCall(std::string target, std::string object, std::string interface, std::string method, DBusPendingCall* & pending)
{
    DBusMessage* msg;

    msg = dbus_message_new_method_call(target.c_str(), object.c_str(), interface.c_str(), method.c_str());
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

bool NetConfReader::GetReply(DBusPendingCall* & pending, DBusMessage* & msg)
{
    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (NULL == msg)
        return false;

    dbus_pending_call_unref(pending);

    return true;
}
