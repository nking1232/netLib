CXX := g++
AR := ar
CFLAGS := -std=c++11 -O2 

main : netServer.o netClient.o
	$(AR) rcs netLib.a netServer.o netClient.o

netServer.o : netServer.cpp
	$(CXX) $(CFLAGS) -c netServer.cpp -o netServer.o

netClient.o : netClient.cpp
	$(CXX) $(CFLAGS) -c netClient.cpp -o netClient.o
	
clean:
	rm netLib.a
	rm netServer.o
	rm netClient.o