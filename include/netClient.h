#ifndef NETCLIENT_H
#define NETCLIENT_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "packet.hpp"
#include "globalExceptions.hpp"


class netClient
{
    public:
        netClient(char*, char*);
        virtual ~netClient();
        int receiveData(char *);
        void sendData(char *, int dataLen);

        void (*onReceive)(Packet, netClient*);
    protected:
    private:
        SOCKET clientSocket = INVALID_SOCKET;
};

#endif // NETCLIENT_H
