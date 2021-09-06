#ifndef ISOCKETUDP_HPP
#define ISOCKETUDP_HPP

#include "common.hpp"

class ISocketUDP
{
public:
    ISocketUDP() = default;
    virtual ~ISocketUDP() = default;

    virtual int init() = 0;
    virtual void sendData(const ByteArray & data) = 0;
    virtual void recvData(ByteArray & data) = 0;
    virtual void closeSock() = 0;
};

#endif // ISOCKETUDP_HPP