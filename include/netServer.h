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
		std::map<int, SOCKET> sessions;
	#elif __linux__
		int ClientSocket = 0, ListenSocket = 0;
		std::map<int, int> sessions;//Linux sockets are just integers
	#endif
        int cid = 1;

};

#endif // NETSERVER_H
