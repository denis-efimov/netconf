#include "core.h"
#include "logger.h"
#include "appconf.h"

int main(void)
{
    //Logger is a singleton so it has to be called
    //before making threads to guarantee that
    //there is only one instance
    Logger::instance();

    AppConf appConf;

    Core core(appConf);

    if(core.Init())
        core.Work();

    return 0;
}
