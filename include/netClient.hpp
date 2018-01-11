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
#ifndef NETCLIENT_H
#define NETCLIENT_H
#define _WIN32_WINNT 0x501
#include <iostream>
#include <map>

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
#endif


#include "globalExceptions.hpp"


class netClient
{
    public:
        netClient(char * server, char * port)
        {
            if(port == NULL)
                throw PortError();//Something is not right with the port throw an exception.

            if(server == NULL)
                //Error
            #ifdef _WIN32
                WSAData Comdata;

                struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;


                int iResult = 1;
                WSAData Comdata;
                iResult = WSAStartup(MAKEWORD(2,2), &Comdata);
                if(iResult != 0)
                    throw WinsockStartUpError();


                ZeroMemory( &hints, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;

                iResult = getaddrinfo(server, port, &hints, &result);
                if(iResult != 0)
                    throw addrInfoError();
                for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
                {

                    // Create a SOCKET for connecting to server
                    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                    ptr->ai_protocol);

                    if (ConnectSocket == INVALID_SOCKET) {
                    //printf("socket failed with error: %ld\n", WSAGetLastError());
                    WSACleanup();
                    exit(1);
                    }

                    // Connect to server.
                    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

                    if (iResult == SOCKET_ERROR)
                    {
                        closesocket(ConnectSocket);
                        ConnectSocket = INVALID_SOCKET;
                        //printf ("The server is down... did not connect");
                        }
                    }

                freeaddrinfo(result);

                if(ConnectSocket == INVALID_SOCKET)
                {
                    //Error here
                }

                u_long iMode = 1;

                iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
                if(iResult == SOCKET_ERROR)
                {

                }

                char value = 1;
                setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
            #elif __linux__
                struct addrinfo hints, *res;

                    memset(&hints, 0, sizeof hints);
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = SOCK_STREAM | SOCK_NONBLOCK;
                hints.ai_protocol = IPPROTO_TCP;

                getaddrinfo(server, port, &hints, &res);

                ConnectSocket = socket(res->ai_family, res->ai_socktype, hints.ai_protocol);

                connect(ConnectSocket, res->ai_addr, res->ai_addrlen);

            #else
                #error "Unknown Compiler"
            #endif
        }


        ~netClient()
        {
            //TODO: Add some deconstruction routines.
            #ifdef _WIN32
            #elif __linux__
            #ifndef __Debug
            #endif
            #else
                #error "Unknown Compiler"
            #endif
        }

        int receiveData()
        {
            #ifdef _WIN32
                char * recvbuf = NULL;
                int iResult = recv(ConnectSocket, recvbuf, 100000, 0);

                if ( iResult == 0 )
                {
                    //Connection was probably lost.
                    if(onReceiveFail == NULL)
                        throw noPointerToFunction();
                    else
                        onReceiveFail(this);
                }
                if(onReceive == NULL)
                    throw noPointerToFunction();
                else
                    onReceive(recvbuf, this);

                return iResult;
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif
            #else
                #error "Unknown Compiler"
            #endif
        }

        int sendData(char *data, int dataLen)
        {
            #ifdef _WIN32
                int iResult = send(ConnectSocket, data, dataLen, 0);
                if(iResult == SOCKET_ERROR)
                {
                    if(onSendFail == NULL)
                        throw noPointerToFunction();
                    else
                        onSendFail(this);
                }
            #elif __linux__
                #ifndef __Debug
                    #error "Unsupported"
                #endif

            #else
                #error "Unknown Compiler"
            #endif
            return 0;
        }


        void disconnect()
        {
            #ifdef _WIN32
                //Of course microsoft has to deviate from almost every standard.
                closesocket(ConnectSocket);
            #elif __linux__
            #ifndef __Debug
                #error "Unsupported"
            #endif
                close(ConnectSocket);

            #else
                #error "Unknown Compiler"
            #endif
        }


        void update()
        {
            receiveData();
        }


        void (*onReceive)(char *, netClient*) = NULL;
		//Let the users code decide what to do.
		void (*onSendFail)(netClient*) = NULL;
		void (*onReceiveFail)(netClient*) = NULL;
    protected:
    private:
      #ifdef _WIN32
        SOCKET ConnectSocket = INVALID_SOCKET;
      #elif __linux__
		int ConnectSocket=0;//Linux sockets are just integers
      #endif
};

#endif // NETCLIENT_H
