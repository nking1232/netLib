#include "include/netServer.h"

netServer::netServer(char * port)
{
    if(port == NULL)
        throw PortError();//Something is not right with the port throw an exception.
    #ifdef _netDebug
        cout <<"Attempting to open server on port: " << port << endl;
    #endif // _netDebug
    WSAData Comdata;

    struct addrinfo * result = NULL;
    struct addrinfo hints;

    int iResult = WSAStartup(MAKEWORD(2,2), &Comdata);
    if(iResult != 0)
    {
        #ifdef _netDebug
         cout << "Failed to start winsock at line: 13 in file: netServer.cpp" << endl;
        #endif
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

    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);

}

netServer::~netServer()
{
    //dtor
}


// accept new connections
bool netServer::acceptNewClient(int id)
{
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
}

// receive incoming data
int netServer::receiveData(int client_id, char * recvbuf)
{
    if( sessions.find(client_id) != sessions.end() )
    {
        SOCKET currentSocket = sessions[client_id];
        int iResult = recv(currentSocket, recvbuf, 1000000, 0);

        return iResult;
    }

    return 0;
}

// send data to all clients
void netServer::sendToAll(char * packets, int totalSize)
{
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
}

void netServer::sendToClient(int client_id, char * packets, int totalSize)
{
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
}

unsigned int netServer::getFirstUnusedId()
{
    unsigned int temp = 0;
    while(true)
    {
        if( sessions.find(temp) != sessions.end())
        {
            temp++;
        }
        else
        {
            break;
        }
    }
    return temp;
}

void netServer::receiveFromClients()
{
    Packet packet;
    map<int, SOCKET>::iterator iter;
    char network_data[1000000];
    int cl_id = 0;
    memset(network_data,0, sizeof(network_data));

    for(iter = sessions.begin(); iter != sessions.end(); iter++)
    {
        cl_id = iter->first;

        cout << "Attempting to Receive from Client ID: " << cl_id << endl;
        int data_length = receiveData(cl_id, network_data);

        if(data_length <= 0)
        {
            //No data was received.
            continue;
        }

        if(data_length < sizeof(packet))
        {
            continue;
        }


        int i = 0;
        while (i < data_length)//Don't know about this one
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);
            if(onReceive == NULL)
            {
                throw noPointerToFunction(); //We need to make sure a function was supplied to handle incoming data.
                return; //Just in case
            }

            cout << "Received Data from Client with ID: " << cl_id << endl;
            onReceive(packet, cl_id, this);
        }
    }
}

char *netServer::receiveFromClientRaw()
{
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
        if(onReceiveRaw == NULL)
        {
            throw noPointerToFunction();
        }
        onReceiveRaw(network_data, iter->first, this);
    }
}

void netServer::disconnectClient(int cl_id)
{
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
}

void netServer::update()
{
    //Accept incoming connections
    if(acceptNewClient(cid))
    {
        cid++;
    }
    receiveFromClients();
}

void netServer::updateRaw()
{
    if(acceptNewClient(cid))
    {
        cid++;
    }
    receiveFromClientRaw();
}
