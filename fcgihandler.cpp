#include "fcgihandler.h"
#include <sstream>
#include <thread>
#include <vector>

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
    socketId = FCGX_OpenSocket(":5000", 0);
    if(socketId < 0)
        return 0;
    else
        return 1;
}

void FcgiHandler::Work()
{
    std::vector<std::thread> threads;

    for(int i=0; i<4; i++)
    {
        threads.push_back(std::thread(&FcgiHandler::ThreadHandle,this));
    }

    for(auto& thread: threads)
        thread.join();
}

void FcgiHandler::ThreadHandle()
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
             << "\r\n"
             << "<html>\n"
             << "  <head>\n"
             << "    <title>Plug</title>\n"
             << "  </head>\n"
             << "  <body>\n"
             << "    <h1>Plug</h1>\n"
             << "  </body>\n"
             << "</html>\n";

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
