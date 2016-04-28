#include <iostream>
#include "fcgio.h"

int main(void)
{
    std::istream in(nullptr);
    std::ostream out(nullptr);
    std::ostream err(nullptr);

    FCGX_Request request;

    FCGX_Init();
    int socketId = FCGX_OpenSocket(":5000", 0);
    if(socketId < 0)
        return 1;

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

    return 0;
}

