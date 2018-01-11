#include <iostream>
#include <exception>

#include <netServer.hpp>
using namespace std;
void dataReceived(char* data, int cid, netServer *theServer)
{
    cout << "DATA: " << data << " FROM " << cid << endl;
    char* packets = "HTTP-Version = HTTP/1.1 200 OK/n/r Date: Mon, 27 Jul 2009 12:28:53 GMT/n/rServer: Overhaul/1.0.0 (Win32/64)/n/rLast-Modified: Wed, 22 Jul 2009 19:15:56 (GMT)/n/rContent-Length: 88/n/rContent-Type: text/html/n/rConnection: Closed/n/r<html><body><h1>Hello, World!</h1></body></html>";
    cout << sizeof(packets) << endl;
    theServer->sendToClient(cid, packets, strlen(packets));
    theServer->disconnectClient(cid);
}
void connectionReceived(int cid, netServer *theServer)
{
    cout << "Connection cid: " << cid << endl;
}
void clientDisconnect(int cid, netServer *theServer)
{

}
int main()
{
    netServer *server = NULL;
    try{
        server = new netServer("80", "127.0.0.1");
    }catch(exception &e)
    {
        cout << e.what() << endl;
    }
    server->onConnect = &connectionReceived;
    server->onReceive = &dataReceived;
    server->onDisconnect = &clientDisconnect;
    while(1)
    {
        server->update();
    }
    return 0;
}
