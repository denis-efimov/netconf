#include "fcgihandler.h"
#include "configuration.h"
#include <sstream>
#include <thread>
#include <vector>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <fstream>

FcgiHandler::FcgiHandler():
    in(nullptr),
    out(nullptr),
    err(nullptr),
    socketId(-1)
{

}

FcgiHandler::~FcgiHandler()
{

}

bool FcgiHandler::Init()
{
    FCGX_Init();
    socketId = FCGX_OpenSocket(Configuration::Instance().GetParams().sockPath.c_str(),
                               Configuration::Instance().GetParams().queueLength);
    if(socketId < 0)
        return 0;
    else
        return 1;
}

void FcgiHandler::Work()
{
    std::vector<std::thread> threads;

    threads.push_back(std::thread(&NetConfReader::ThreadFunc, &netConfReader, std::chrono::seconds(60)));

    for(int i=0; i<Configuration::Instance().GetParams().threadsNum; i++)
    {
        threads.push_back(std::thread(&FcgiHandler::ThreadFunc,this, std::ref(netConfReader)));
    }

    for(auto& thread: threads)
        thread.join();
}

void FcgiHandler::ThreadFunc(NetConfReader & netConfReader)
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

        boost::property_tree::json_parser::write_json(outString, netConfReader.GetNetConfiguration(), false);

        PrintOut(request, outString.str());
    }
}

void FcgiHandler::PrintOut(FCGX_Request & request, const std::string &str)
{
    std::lock_guard<std::mutex> lock(outStreamMutex);

    fcgi_streambuf out_fcgi_streambuf(request.out);
    out.rdbuf(&out_fcgi_streambuf);

    out << str;
}
