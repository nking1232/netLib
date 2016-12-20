#ifndef GLOBALEXCEPTIONS_HPP_INCLUDED
#define GLOBALEXCEPTIONS_HPP_INCLUDED
#include <exception>
struct PortError : public exception {
   const char * what () const throw () {
      return "Invalid Port or No Port specified";
   }
};
//Winsock sepcific
struct WinsockStartUpError : public exception {
    const char * what () const throw () {
        return "Winsock failed to initialize";
    }
};

struct sockSendError : public exception {
    const char * what () const throw () {
        return "send failed possible socket error";
    }
};

struct sockWriteError : public exception {
    const char * what () const throw () {
	return "write failed possible socket error";
    }
};

struct addrInfoError : public exception {
    const char * what () const throw () {
        return "addrInfoFailed";
    }
};

struct listenSocketError : public exception {
    const char * what () const throw () {
        return "Listen Socket Error";
    }
};

struct noPointerToFunction : public exception{
    const char * what () const throw () {
        return "Pointer to function not supplied";
    }
};


#endif // GLOBALEXCEPTIONS_HPP_INCLUDED
