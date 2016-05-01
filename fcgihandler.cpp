#include "fcgihandler.h"
#include "fcgio.h"

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
    FCGX_Request request;

    FCGX_InitRequest(&request, socketId, 0);

    while (FCGX_Accept_r(&request) == 0)
    {
        fcgi_streambuf in_fcgi_streambuf(request.in);
        fcgi_streambuf out_fcgi_streambuf(request.out);
        fcgi_streambuf err_fcgi_streambuf(request.err);

        in.rdbuf(&in_fcgi_streambuf);
        out.rdbuf(&out_fcgi_streambuf);
        err.rdbuf(&err_fcgi_streambuf);

        out << "Content-type: text/html\r\n"
             << "\r\n"
             << "<html>\n"
             << "  <head>\n"
             << "    <title>Plug</title>\n"
             << "  </head>\n"
             << "  <body>\n"
             << "    <h1>Plug</h1>\n"
             << "  </body>\n"
             << "</html>\n";
    }
}
