#ifndef NETSERVER_H
#define NETSERVER_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <chrono>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
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
        char *receiveFromClientRaw();
        void disconnectClient(int cl_id);
        void update();
        void updateRaw();

        void (*onReceive) (Packet, int, netServer*) = NULL; //A pointer to a function called when we receive a packet.
        void (*onReceiveRaw) (char*, int, netServer*) = NULL;//Same as above but for use with raw connections.
        void (*onConnect) (int, netServer*);
        void (*onDisconnect)(int, netServer*);
    protected:
    private:
        SOCKET ClientSocket = INVALID_SOCKET, ListenSocket = INVALID_SOCKET;
        std::map<int, SOCKET> sessions;
        int cid = 1;

};

#endif // NETSERVER_H
