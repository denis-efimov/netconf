#ifndef CORE_H
#define CORE_H

#include "fcgihandler.h"
#include "appconf.h"
#include "netconfreader.h"
#include "dbushandler.h"

class Core
{
public:
    Core(const AppConf & appConf);
    bool Init();
    void Work();

private:
    FcgiHandler fcgiHandler;
    NetConfReader netConfReader;
    DBusHandler dBusHandler;
    int fcgiThreadsNum;
};

#endif // CORE_H
