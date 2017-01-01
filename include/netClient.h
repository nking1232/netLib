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
        int receiveData();
        int sendData(char *, int dataLen);
	void disconnect();
	void update();

        void (*onReceive)(char *, netClient*) = NULL;
		//Let the users code decide what to do.
		void (*onSendFail)(netClient*) = NULL;
		void (*onReceiveFail)(netClient*) = NULL;
    protected:
    private:
      #ifdef _WIN32
        SOCKET ConnectSocket = INVALID_SOCKET;
      #endif //_WIN32
      #ifdef linux
		int ConnectSocket=0;//Linux sockets are just integers
      #endif //linux
};

#endif // NETCLIENT_H
