#include "configuration.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

const char * Configuration::CONF_FILENAME = "conf.json";

const Configuration & Configuration::Instance()
{
    static const Configuration instance;
    return instance;
}

const Configuration::paramsStruct & Configuration::GetParams() const
{
    return params;
}

Configuration::Configuration()
{
    if(!LoadConfiguration())
        CreateConfiguration();
}

bool Configuration::LoadConfiguration()
{
    std::ifstream file(CONF_FILENAME);
    if(file.is_open())
    {
        using namespace boost::property_tree;

        ptree pt;
        json_parser::read_json(file, pt);

        params.sockPath.assign(pt.get<std::string>("socket"));
        params.queueLength = pt.get<int>("queue");
        params.threadsNum = pt.get<int>("threads");

        return true;
    }
    return false;
}

bool Configuration::CreateConfiguration()
{
    using namespace boost::property_tree;

    ptree pt;
    pt.put("socket", params.sockPath);
    pt.put("queue", params.queueLength);
    pt.put("threads", params.threadsNum);

    std::ofstream file(CONF_FILENAME);
    json_parser::write_json(file, pt);

    return true;
}
