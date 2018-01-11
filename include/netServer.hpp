/*
Copyright (C) 2017  Nathan King

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NETSERVER_H
#define NETSERVER_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <chrono>
#include <map>
#include <list>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#elif __linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#elif __APPLE__
		#error "Not Supported"
		#include "TargetConditionals.h"
		#if TARGET_IPHONE_SIMULATOR
			#warning "Compiling under IPhone Simulator"
			//iOS Simulator
		#elif TARGET_OS_IPHONE
			#warning "Compiling under IPhone"
			//iOS device
		#elif TARGET_OS_MAC
			#warning "Compiling under mac"
			//Mac OS
		#else
			#error "Unknown Apple platform"
		#endif
#elif __unix__
		#error "Not Supported"
		#warning "Compiling under __unix__"
#else
		#error "Unknown Compiler"
#endif


#include "globalExceptions.hpp"

class netServer
{
    public:
        netServer(char * port, char *ip)
        {
            if(port == NULL)
                throw PortError();//Something is not right with the port throw an exception.
            #ifdef _WIN32
                #warning "Building in WIN32 mode"


                WSAData Comdata;

                struct addrinfo * result = NULL;
                struct addrinfo hints;

                int iResult = WSAStartup(MAKEWORD(2,2), &Comdata);
                if(iResult != 0)
                {
                    throw WinsockStartUpError();//We have a problem let's throw an exception.
                }

                ZeroMemory(&hints, sizeof(hints));
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
                hints.ai_flags = AI_PASSIVE;
                string prt;
                prt += port;
                iResult = getaddrinfo(NULL, port, &hints, &result);
                if(iResult != 0)
                    throw addrInfoError();


                ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
                if(ListenSocket == SOCKET_ERROR)
                    throw listenSocketError();

                u_long iMode = 1;
                iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);
                if(iResult != 0)
                    throw nonBlockingModeError();

                iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
                if(iResult != 0)
                    throw bindError();

                freeaddrinfo(result);
                iResult = listen(ListenSocket, SOMAXCONN);

            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
                if(port == NULL)
                throw PortError();
                struct sockaddr_in serv_addr;
                ListenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);


                serv_addr.sin_family = AF_INET;
                serv_addr.sin_addr.s_addr = INADDR_ANY;
                serv_addr.sin_port = htons(atoi(port));

                if (bind(ListenSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                    throw bindError();

                listen(ListenSocket,5);


            #else
                #error "Unknown Compiler"
            #endif
        }

        ~netServer()
        {
            //TODO: Add some deconstruction routines.
            #ifdef _WIN32
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
            #elif __APPLE__
                #error "Not Supported"
                #include "TargetConditionals.h"
                #if TARGET_IPHONE_SIMULATOR
                    #warning "Compiling under IPhone Simulator"
                    //iOS Simulator
                #elif TARGET_OS_IPHONE
                    #warning "Compiling under IPhone"
                    //iOS device
                #elif TARGET_OS_MAC
                    #warning "Compiling under mac"
                    //Mac OS
                #else
                    #error "Unknown Apple platform"
                #endif
            #elif __unix__
                #error "Not Supported"
                #warning "Compiling under __unix__"
            #else
                #error "Unknown Compiler"
            #endif
        }


        // accept new connections

        bool acceptNewClient(int id)
        {
            #ifdef _WIN32
                // if client waiting, accept the connection and save the socket
                ClientSocket = accept(ListenSocket,NULL,NULL);

                if (ClientSocket != INVALID_SOCKET)
                {
                    //disable nagle on the client's socket
                    char value = 1;
                    setsockopt( ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );

                    // insert new client into session id table
                    #ifdef _netDebug
                        cout << "Accepted client with ID: " << id << endl;
                    #endif // _netDebug
                    sessions.insert( pair<int, SOCKET>(id, ClientSocket) );
                    if(onConnect != NULL)
                    {
                        onConnect(id, this);
                    }
                    return true;
                }

                return false;

            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
                struct sockaddr_in cli_addr;
                socklen_t clilen = sizeof(cli_addr);
                ClientSocket = accept(ListenSocket, (struct sockaddr *) &cli_addr, &clilen);
                if(ClientSocket > 0)
                {
                    //We possibly have a connection here.
                    fcntl(ClientSocket, F_SETFL, O_NONBLOCK);

                    sessions.insert( pair<int, int>(id, ClientSocket));
                    if(onConnect != NULL)
                    {
                        onConnect(id, this);
                    }
                    return true;
                }
                return false;
            #else
                #error "Unknown Compiler"
            #endif
        }
        // receive incoming data
        int receiveData(int client_id, char * recvbuf)
        {
            #ifdef _WIN32
                if( sessions.find(client_id) != sessions.end() )
                {
                    SOCKET currentSocket = sessions[client_id];
                    int iResult = recv(currentSocket, recvbuf, 1000000, 0);

                    return iResult;
                }

                return 0;
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
                if(sessions.find(client_id) != sessions.end() )
                {
                    int currentSocket = sessions[client_id];
                    int iResult = recv(currentSocket, recvbuf, 100000, 0);//Same as for windows

                    return iResult;
                }
            #else
                #error "Unknown Compiler"
            #endif
        }


        void sendToAll(char * packets, int totalSize)
        {
            #ifdef _WIN32
            SOCKET currentSocket;
            std::map<int, SOCKET>::iterator iter;
            int iSendResult;

            for (iter = sessions.begin(); iter != sessions.end(); iter++)
            {
                currentSocket = iter->second;
                iSendResult = send(currentSocket, packets, totalSize, 0);
                if(iSendResult == SOCKET_ERROR)
                {
                    //We have a problem
                    throw sockSendError();
                }
            }
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
                int currentSocket;
                std::map<int, int>::iterator iter;
                int iSendResult;

                for(iter = sessions.begin(); iter != sessions.end(); iter++)
                {
                    currentSocket = iter->second;
                    iSendResult = write(currentSocket, packets, totalSize);
                    if(iSendResult < 0)
                    {
                    }
                }
            #else
                #error "Unknown Compiler"
            #endif
        }


        void sendToClient(int client_id, char * packets, int totalSize)
        {
            #ifdef _WIN32
                SOCKET currentSocket;
                if( sessions.find(client_id) != sessions.end() )
                {
                    currentSocket = sessions[client_id];
                    int iSendResult = send(currentSocket, packets, totalSize, 0);
                    if(iSendResult == SOCKET_ERROR)
                    {
                        //This occurs when the connection is interupted;
                        throw sockSendError();
                    }

                }
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
            #else
                #error "Unknown Compiler"
            #endif

        }


        void receiveFromClients()
        {
            #ifdef _WIN32
                map<int, SOCKET>::iterator iter;
                char network_data[1000000];

                memset(network_data,0,sizeof(network_data));

                for(iter = sessions.begin(); iter != sessions.end(); iter++)
                {
                    int data_length = receiveData(iter->first, network_data);

                    if(data_length <= 0)
                    {
                        continue;
                    }
                    if(onReceive== NULL)
                    {
                        throw noPointerToFunction();
                    }
                    onReceive(network_data, iter->first, this);
                }
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
            #else
                #error "Unknown Compiler"
            #endif

        }


        void disconnectClient(int cl_id)
        {
            #ifdef _WIN32
                map<int, SOCKET>::iterator it = sessions.find(cl_id);
                if(it != sessions.end())
                {
                    SOCKET currentSocket = it->second;
                    if(currentSocket != SOCKET_ERROR)
                    {
                        closesocket(currentSocket);
                        //TODO: Schedule for erasure to keep program from crashing.
                        //sessions.erase(it);
                        deletionSchedule.push_back(cl_id);
                        if(onDisconnect != NULL)
                        {
                            onDisconnect(cl_id+1, this);
                        }
                    }
                }

            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
                if(sessions.find(cl_id) != sessions.end())
                {
                    int currentSocket = sessions[cl_id];
                    if(currentSocket < 0)
                    {
                        close(currentSocket);
                        sessions.erase(cl_id);
                        if(onDisconnect != NULL)
                        {
                            onDisconnect(cl_id, this);
                        }
                    }
                }
            #else
                #error "Unknown Compiler"
            #endif
        }


        void update()
        {
            if(acceptNewClient(cid))
            {
                cid++;
            }
            receiveFromClients();
        }


        void (*onReceive) (char*, int, netServer*) = NULL; //A pointer to a function called when we receive a data;
        void (*onConnect) (int, netServer*) = NULL;
        void (*onDisconnect)(int, netServer*) = NULL;
    protected:
    private:
	#ifdef _WIN32
        SOCKET ClientSocket = INVALID_SOCKET, ListenSocket = INVALID_SOCKET;
		std::map<int, SOCKET> sessions;
	#elif __linux__
		int ClientSocket = 0, ListenSocket = 0;
		std::map<int, int> sessions;//Linux sockets are just integers
	#endif
        int cid = 1;
        std::list<int> deletionSchedule;
};

#endif // NETSERVER_H
