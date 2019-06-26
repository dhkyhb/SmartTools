#pragma once

#ifndef SMARTTOOLS_SMART_UTILS_SOCKET_BASED_ON_LINUX_HPP
#define SMARTTOOLS_SMART_UTILS_SOCKET_BASED_ON_LINUX_HPP

#include "../kernel.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <sys/socket.h>

namespace smart::utils::soket::linux
{

typedef struct
{
    Jint socket;
    sockaddr_in sockaddrIn;
    timeval readTimeouts;
    timeval writeTimeouts;

    Jint port;
    const Jchar *address;
} SocketAttr;

class Socket
{
public:
    Socket() :
        mSocketAttr{}
    {}

    ~Socket() = default;

    Jbool Connect()
    {
        Jint i = 0;

        auto &&hostT = gethostbyname(this->mSocketAttr.address);
        if (hostT == nullptr)
            return false;

        while ((*hostT).h_addr_list[i] != nullptr)
        {
            Log::Instance().Print<LogType::DEBUG>(
                "address parse: %s",
                inet_ntoa(*reinterpret_cast<in_addr *>((*hostT).h_addr_list[i]))
            );
            ++i;
        }

        if (i < 1)
            return false;

        this->mSocketAttr.socket = socket(AF_INET, SOCK_STREAM, 0);
        this->mSocketAttr.sockaddrIn.sin_family = AF_INET;
        this->mSocketAttr.sockaddrIn.sin_port = htons(this->mSocketAttr.port);
        this->mSocketAttr.sockaddrIn.sin_addr = *reinterpret_cast<in_addr *>((*hostT).h_addr_list[0]);

        if (connect(
            this->mSocketAttr.socket,
            reinterpret_cast<sockaddr *>(&this->mSocketAttr.sockaddrIn),
            sizeof(this->mSocketAttr.sockaddrIn)
        ) == -1)
            return false;

        if (setsockopt(
            this->mSocketAttr.socket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            &this->mSocketAttr.writeTimeouts,
            sizeof(this->mSocketAttr.writeTimeouts)
        ) != 0)
            return false;

        if (setsockopt(
            this->mSocketAttr.socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &this->mSocketAttr.readTimeouts,
            sizeof(this->mSocketAttr.readTimeouts)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mSocketAttr.socket, F_GETFL, 0);
        fcntl(this->mSocketAttr.socket, F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("socket initialization successful");
        return true;
    }

    void DisConnect()
    {
        shutdown(this->mSocketAttr.socket, SHUT_RDWR);
    }

    Socket &SetWriteTimeoutsNanoSecounds(Jint times)
    {
        this->mSocketAttr.writeTimeouts.tv_usec = times;
        return (*this);
    }

    Socket &SetReadTimeoutsNanoSecounds(Jint times)
    {
        this->mSocketAttr.readTimeouts.tv_usec = times;
        return (*this);
    }

    Socket &SetWriteTimeoutsSecounds(Jint times)
    {
        this->mSocketAttr.writeTimeouts.tv_sec = times;
        return (*this);
    }

    Socket &SetReadTimeoutsSecounds(Jint times)
    {
        this->mSocketAttr.readTimeouts.tv_sec = times;
        return (*this);
    }

    Socket &SetPort(Jint port)
    {
        this->mSocketAttr.port = port;
        return (*this);
    }

    Socket &SetAddress(const Jchar *v)
    {
        this->mSocketAttr.address = v;
        return (*this);
    }

    Jint GetSocket()
    {
        return this->mSocketAttr.socket;
    }

private:
    SocketAttr mSocketAttr;

    Jbool Check()
    {
        return (this->mSocketAttr.address != nullptr);
    }
};

}

#endif //SMARTTOOLS_SMART_UTILS_SOCKET_BASED_ON_LINUX_HPP
