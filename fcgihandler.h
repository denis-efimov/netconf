#ifndef FCGIHANDLER_H
#define FCGIHANDLER_H

#include "fcgio.h"
#include "appconf.h"
#include "netconfreader.h"
#include <iostream>
#include <mutex>

class FcgiHandler
{
public:
    FcgiHandler();
    ~FcgiHandler();

    bool Init();
    void Work();

private:
    void ThreadFunc(NetConfReader & netConfReader);
    void PrintOut(FCGX_Request & request, const std::string & str);

private:
    AppConf appConf;
    NetConfReader netConfReader;

    std::istream in;
    std::ostream out;
    std::ostream err;

    int socketId;
    std::mutex fcgxAcceptMutex;
    std::mutex outStreamMutex;
};

#endif // FCGIHANDLER_H
