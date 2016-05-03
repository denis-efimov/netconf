#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

class Configuration
{
private:
    struct paramsStruct;

public:
    static const Configuration & Instance();
    const paramsStruct & GetParams() const;

private:
    Configuration();
    Configuration( const Configuration &){}
    Configuration & operator=( Configuration & ){}
    bool LoadConfiguration();
    bool CreateConfiguration();

    struct paramsStruct
    {
        paramsStruct()
        {
            sockPath = ":5000";
            queueLength = 0;
            threadsNum = 1;
        }

        std::string sockPath;
        int queueLength;
        int threadsNum;
    }params;

    static const char * CONF_FILENAME;
};

#endif // CONFIGURATION_H
