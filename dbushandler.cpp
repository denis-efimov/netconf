#include "dbushandler.h"

#include "logger.h"

DBusHandler::DBusHandler(const std::string &name):
    isConnected(false),
    _name(name)
{

}

bool DBusHandler::Connect()
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

    dbus_bus_request_name(conn, _name.data(), DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err))
    {
        //Name Error
        Logger::instance() << err.message;
        dbus_error_free(&err);
        return false;
    }

    isConnected = true;
    return true;
}

bool DBusHandler::MethodCall(const char *target, const char *object, const char *interface, const char *method, DBusPendingCall* & pending)
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

bool DBusHandler::PropertyRequest(const char * target, const char * object, const char * interface, const char * property, DBusPendingCall* & pending)
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

bool DBusHandler::GetReply(DBusPendingCall* & pending, DBusMessage* & msg)
{
    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (NULL == msg)
        return false;

    dbus_pending_call_unref(pending);

    return true;
}

std::string DBusHandler::GetString(DBusMessageIter & iter)
{
    char * cstr;
    dbus_message_iter_get_basic(&iter, &cstr);
    return std::string(cstr);
}

int DBusHandler::GetInt(DBusMessageIter &iter)
{
    int num;
    dbus_message_iter_get_basic(&iter, &num);
    return num;
}
