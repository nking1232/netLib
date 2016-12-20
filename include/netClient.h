#ifndef NETCLIENT_H
#define NETCLIENT_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <map>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif //_WIN32

#ifdef linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#endif //linux

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
      #ifdef _WIN32
        SOCKET clientSocket = INVALID_SOCKET;
      #endif //_WIN32
      #ifdef linux
		int clientSocket=0;//Linux sockets are just integers
      #endif //linux
};

#endif // NETCLIENT_H
