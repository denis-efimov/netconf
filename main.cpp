#include "fcgihandler.h"
#include "logger.h"

int main(void)
{
    //Logger is a singleton so it has to be called
    //before making threads to guarantee that
    //there is only one instance
    Logger::instance();

    FcgiHandler fcgiHandler;

    if(fcgiHandler.Init())
        fcgiHandler.Work();

    return 0;
}
