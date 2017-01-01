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
#elif linux
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
	#warning "Compiling under __linux__"
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
#ifdef _WIN32
int netClient::receiveData() 
{
	
	char * recvbuf = 0;
	ZeroMemory( recvbuf, sizeof(recvbuf));
    int iResult = recv(ConnectSocket, recvbuf, 100000, 0);

    if ( iResult == 0 )
    {
        //Connection was probably lost.
    }
	if(onReceive == NULL)
	{
		throw noPointerToFunction();
	}
	onReceive(recvbuf, this);
	
    return iResult;
}
#endif //_WIN32
#ifdef linux
#endif //linux

#ifdef _WIN32
int netClient::sendData(char *data, int dataLen)
{
	int iResult = send(ConnectSocket, data, dataLen, 0);
	if(iResult == SOCKET_ERROR)
	{
		
	}
}
#endif //_WIN32
#ifdef linux
#endif //linux

#ifdef _WIN32
void netClient::disconnect()
{
	//Of course microsoft has to deviate from almost every standard.
	closesocket(ConnectSocket);
}
#endif //_WIN32
#ifdef linux
void netClient::disconnect()
{
	close(ConnectSocket);
}
#endif //linux


void netClient::update()
{
	receiveData();
}