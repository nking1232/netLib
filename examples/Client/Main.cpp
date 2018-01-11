#include <iostream>
#include <exception>

#include <netLib.hpp>

using namespace std;

int main()
{
    netClient *theClient = NULL;
    try{
        theClient = new netClient("127.0.0.1", "80");
    }catch(exception &e){
        cout << e.what() << endl;
    }
    while(1)
    {
        theClient->update();
    }
}
