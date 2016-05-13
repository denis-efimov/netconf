#include "appconf.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

const char * AppConf::CONF_FILENAME = "conf.json";

AppConf::AppConf()
{
    if(!LoadConfiguration())
        CreateConfiguration();
}

const AppConf::FcgiParamsStruct & AppConf::GetFcgiParams() const
{
    return fcgiParams;
}

const AppConf::DBusParamsStruct &AppConf::GetDBusParams() const
{
    return dBusParams;
}

bool AppConf::LoadConfiguration()
{
    std::ifstream file(CONF_FILENAME);
    if(file.is_open())
    {
        using namespace boost::property_tree;

        ptree mainPT;
        json_parser::read_json(file, mainPT);

        if(const boost::optional<ptree&> fcgiParamsPT = mainPT.get_child_optional("fcgi"))
        {
            if(const boost::optional<std::string> socket = fcgiParamsPT.get().get_optional<std::string>("socket"))
                fcgiParams.sockPath.assign(socket.get());
            if(const boost::optional<int> queue = fcgiParamsPT.get().get_optional<int>("queue"))
                fcgiParams.queueLength = queue.get();
            if(const boost::optional<int> threads = fcgiParamsPT.get().get_optional<int>("threads"))
                fcgiParams.threadsNum = threads.get();
        }

        if(const boost::optional<ptree&> dBusParamsPT = mainPT.get_child_optional("dbus"))
        {
            if(const boost::optional<int> period = dBusParamsPT.get().get_optional<int>("period"))
                dBusParams.period = period.get();
            if(const boost::optional<std::string> name = dBusParamsPT.get().get_optional<std::string>("name"))
                dBusParams.name.assign(name.get());
        }

        return true;
    }
    return false;
}

bool AppConf::CreateConfiguration()
{
    using namespace boost::property_tree;

    ptree mainPT;

    ptree fcgiParamsPT;
    fcgiParamsPT.put("socket", fcgiParams.sockPath);
    fcgiParamsPT.put("queue", fcgiParams.queueLength);
    fcgiParamsPT.put("threads", fcgiParams.threadsNum);

    ptree dBusParamsPT;
    dBusParamsPT.put("period", dBusParams.period);
    dBusParamsPT.put("name", dBusParams.name);

    mainPT.put_child("fcgi", fcgiParamsPT);
    mainPT.put_child("dbus", dBusParamsPT);

    std::ofstream file(CONF_FILENAME);
    json_parser::write_json(file, mainPT);

    return true;
}
