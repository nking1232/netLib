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
#ifndef GLOBALEXCEPTIONS_HPP_INCLUDED
#define GLOBALEXCEPTIONS_HPP_INCLUDED

#include <exception>
using namespace std;

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

struct bindError : public exception {
	const char * what () const throw () {
		return "Failed to bind";
	}
};

struct nonBlockingModeError : public exception {
	const char * what () const throw () {
		return "Unable to set a socket to non-blocking mode";
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

struct connectionFailed : public exception{
	const char * what () const throw ()
	{
		return "Connection failed";
	}
};


#endif // GLOBALEXCEPTIONS_HPP_INCLUDED
