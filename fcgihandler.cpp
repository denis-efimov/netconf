#include "fcgihandler.h"
#include <sstream>
#include <thread>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

FcgiHandler::FcgiHandler(NetConfReader & netConfReader, const AppConf::FcgiParamsStruct & params):
    in(nullptr),
    out(nullptr),
    err(nullptr),
    socketId(-1),
    _params(params),
    _netConfReader(netConfReader)
{

}

FcgiHandler::~FcgiHandler()
{

}

bool FcgiHandler::Init()
{
    FCGX_Init();
    socketId = FCGX_OpenSocket(_params.sockPath.c_str(),
                               _params.queueLength);
    if(socketId < 0)
        return 0;
    else
        return 1;
}

void FcgiHandler::ThreadFunc()
{
    FCGX_Request request;

    FCGX_InitRequest(&request, socketId, 0);

    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(fcgxAcceptMutex);
            if(FCGX_Accept_r(&request) != 0)
                return;
        }

        std::ostringstream outString;
        outString << "Content-type: text/html\r\n"
                  << "\r\n";

        boost::property_tree::json_parser::write_json(outString, _netConfReader.GetNetConfiguration(), false);

        PrintOut(request, outString.str());

        FCGX_Finish_r(&request);
    }
}

void FcgiHandler::PrintOut(FCGX_Request & request, const std::string &str)
{
    std::lock_guard<std::mutex> lock(outStreamMutex);

    fcgi_streambuf out_fcgi_streambuf(request.out);
    out.rdbuf(&out_fcgi_streambuf);

    out << str;
}
