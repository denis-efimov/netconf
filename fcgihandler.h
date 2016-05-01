#ifndef FCGIHANDLER_H
#define FCGIHANDLER_H

#include <iostream>

class FcgiHandler
{
public:
    FcgiHandler();
    ~FcgiHandler();

    bool Init();
    void Work();

private:
    std::istream in;
    std::ostream out;
    std::ostream err;

    int socketId;
};

#endif // FCGIHANDLER_H
