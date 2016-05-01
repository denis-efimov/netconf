#include "fcgihandler.h"

int main(void)
{
    FcgiHandler fcgiHandler;

    if(fcgiHandler.Init())
        fcgiHandler.Work();

    return 0;
}
