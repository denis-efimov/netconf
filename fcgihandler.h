#ifndef FCGIHANDLER_H
#define FCGIHANDLER_H

#include "fcgio.h"
#include "netconfreader.h"
#include "appconf.h"
#include <iostream>
#include <mutex>

class FcgiHandler
{
public:
    FcgiHandler(NetConfReader & netConfReader, const AppConf::FcgiParamsStruct & params);
    ~FcgiHandler();

    bool Init();
    void ThreadFunc();

private:
    void PrintOut(FCGX_Request & request, const std::string & str);

private:
    std::istream in;
    std::ostream out;
    std::ostream err;

    int socketId;
    std::mutex fcgxAcceptMutex;
    std::mutex outStreamMutex;

    AppConf::FcgiParamsStruct _params;
    NetConfReader & _netConfReader;
};

#endif // FCGIHANDLER_H
