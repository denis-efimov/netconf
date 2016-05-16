#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

class AppConf
{
public:
    struct FcgiParamsStruct;
    struct DBusParamsStruct;

    AppConf();
    const FcgiParamsStruct & GetFcgiParams() const;
    const DBusParamsStruct & GetDBusParams() const;

    struct FcgiParamsStruct
    {
        FcgiParamsStruct()
        {
            sockPath = ":5000";
            queueLength = 0;
            threadsNum = 1;
        }

        std::string sockPath;
        int queueLength;
        int threadsNum;
    }fcgiParams;

    struct DBusParamsStruct
    {
        DBusParamsStruct()
        {
            period = 60;
            name = "test.method.server";
        }

        int period;
        std::string name;
    }dBusParams;

private:
    bool LoadConfiguration();
    bool CreateConfiguration();

    static const char * CONF_FILENAME;
};

#endif // CONFIGURATION_H
