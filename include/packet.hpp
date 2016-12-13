#ifndef _PACKET_HPP_INCLUDED_
#define _PACKET_HPP_INCLUDED_
#include <string>
#include <cstring>

using namespace std;


struct Packet{
    int type;

    char dat[512];
    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};

#endif // _PACKET_HPP_INCLUDED_
