#include "include/netClient.h"

#ifdef _WIN32
netClient::netClient(char * server, char * port)
{
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
        clientSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        if (clientSocket == INVALID_SOCKET) {
            //printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        // Connect to server.
        iResult = connect( clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
            //printf ("The server is down... did not connect");
        }
    }

    freeaddrinfo(result);

    if(clientSocket == INVALID_SOCKET)
    {
        //Error here
    }

    u_long iMode = 1;

    iResult = ioctlsocket(clientSocket, FIONBIO, &iMode);
    if(iResult == SOCKET_ERROR)
    {

    }

    char value = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}
#endif //_WIN32

#ifdef linux
netClient::netClient(char *server, char *port)
{
  
}
#endif //linux
netClient::~netClient()
{
    //dtor
}
