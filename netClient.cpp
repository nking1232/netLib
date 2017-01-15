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
#include "include/netClient.h"


netClient::netClient(char * server, char * port)
{
	#ifdef _WIN32
     WSAData Comdata;

    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;


    int iResult = 1;

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
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

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
		#ifndef __Debug
			#error "Unsupported"
		#endif
	if(port == NULL)
	{
		throw portError();
	}
	
	ConnectSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
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


netClient::~netClient()
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
int netClient::receiveData() 
{
	#ifdef _WIN32
		char * recvbuf = 0;
		//If we do not zero out our receive buffer stange things will happen
		ZeroMemory( recvbuf, sizeof(recvbuf));
		int iResult = recv(ConnectSocket, recvbuf, 100000, 0);

		if ( iResult == 0 )
		{
			//Connection was probably lost.
			if(onReceiveFail == NULL)
			{
				throw noPointerToFunction();
			}
			onReceiveFail(this);
		}
		if(onReceive == NULL)
		{
			throw noPointerToFunction();
		}
		onReceive(recvbuf, this);
	
		return iResult;
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

int netClient::sendData(char *data, int dataLen)
{
	#ifdef _WIN32
		int iResult = send(ConnectSocket, data, dataLen, 0);
		if(iResult == SOCKET_ERROR)
		{
			if(onSendFail == NULL)
			{
				throw noPointerToFunction();
			}
			onSendFail(this);
		}
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


void netClient::disconnect()
{
	#ifdef _WIN32
		//Of course microsoft has to deviate from almost every standard.
		closesocket(ConnectSocket);
	#elif __linux__
		#ifndef __Debug
			#error "Unsupported"
		#endif
		close(ConnectSocket);
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


void netClient::update()
{
	receiveData();
}