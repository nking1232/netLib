#ifndef NETSERVER_H
#define NETSERVER_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <chrono>
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

class netServer
{
    public:
        netServer(char * port);
        virtual ~netServer();
        bool acceptNewClient(int id);
        int receiveData(int client_id, char * recvbuf);
        void sendToClient(int client_id, char * packets, int totalSize);
        void sendToAll(char * packets, int totalSize);
        unsigned int getFirstUnusedId();
        void receiveFromClients();
        void disconnectClient(int cl_id);
        void update();

        void (*onReceive) (char*, int, netServer*) = NULL; //A pointer to a function called when we receive a data;
        void (*onConnect) (int, netServer*) = NULL;
        void (*onDisconnect)(int, netServer*) = NULL;
    protected:
    private:
	#ifdef _WIN32
        SOCKET ClientSocket = INVALID_SOCKET, ListenSocket = INVALID_SOCKET;
	#endif //_WIN32
	#ifdef linux
	int ClientSocket = 0, ListenSocket = 0;
	#endif //linux
	#ifdef _WIN32
        std::map<int, SOCKET> sessions;
	#endif //_WIN32
	#ifdef linux
	std::map<int, int> sessions;//Linux sockets are just integers
	#endif //linux
        int cid = 1;

};

#endif // NETSERVER_H
