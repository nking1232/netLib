#include "include/netServer.h"



netServer::netServer(char * port)
{
	#ifdef _WIN32
		#warning "Compiling netServer::netServer() under windows"
		if(port == NULL)
			throw PortError();//Something is not right with the port throw an exception.
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
		{
			throw nonBlockingModeError();
		
		}

		iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if(iResult != 0)
		{
			throw bindError();
		}

		freeaddrinfo(result);
		iResult = listen(ListenSocket, SOMAXCONN);

	#elif __linux__
		#warning "Compiling under linux"
		if(port == NULL)
			throw PortError();
		struct sockaddr_in serv_addr;
		ListenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
  
  
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(atoi(port));
  
		if (bind(ListenSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		{
		//Error binidng
		throw bindError();
		}
		listen(ListenSocket,5);
  
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
	
	#else
		#error "Unknown Compiler"
	#endif
}

netServer::~netServer()
{
	//TODO: Add some deconstruction routines.
    #ifdef _WIN32
	#elif __linux__
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

bool netServer::acceptNewClient(int id)
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
// receive incoming data
int netServer::receiveData(int client_id, char * recvbuf)
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
		if(sessions.find(client_id) != sessions.end() )
		{
			int currentSocket = sessions[client_id];
			int iResult = recv(currentSocket, recvbuf, 100000, 0);//Same as for windows
    
			return iResult;
		}
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


void netServer::sendToAll(char * packets, int totalSize)
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


void netServer::sendToClient(int client_id, char * packets, int totalSize)
{
	#ifdef _WIN32
		SOCKET currentSocket;
		if( sessions.find(client_id) != sessions.end() )
		{
			currentSocket = sessions[client_id];
			int iSendResult = send(currentSocket, packets, totalSize, 0);
			if(iSendResult == SOCKET_ERROR)
			{
				throw sockSendError();
			}

		}
	#elif __linux__
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


void netServer::receiveFromClients()
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


void netServer::disconnectClient(int cl_id)
{
	#ifdef _WIN32
		if(sessions.find(cl_id) != sessions.end())
		{
			SOCKET currentSocket = sessions[cl_id];
			if(currentSocket != SOCKET_ERROR)
			{
				closesocket(currentSocket);
				sessions.erase(cl_id);
				if(onDisconnect != NULL)
				{
					onDisconnect(cl_id, this);
				}
			}
		}
	#elif __linux__
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


void netServer::update()
{
    if(acceptNewClient(cid))
    {
        cid++;
    }
    receiveFromClients();
}
