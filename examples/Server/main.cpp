#include <iostream>
#include <exception>

#include <netServer.hpp>
using namespace std;
void dataReceived(char* data, int cid, netServer *theServer)
{
    cout << "DATA: " << data << " FROM " << cid << endl;
    char packets[] = "Boop";
    cout << packets << endl;
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
