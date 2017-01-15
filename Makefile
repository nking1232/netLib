CXX := g++
AR := ar
CFLAGS := -std=c++11 -O2 

main : netServer.o netClient.o
	$(AR) rcs netLib.a netServer.o netClient.o

debug : netServer.o_debug netClient.o_debug
	$(AR) rcs netLib.a netServer.o netClient.o
	
netServer.o_debug : netServer.cpp
	$(CXX) -D__Debug $(CFLAGS) -c netServer.cpp -o netServer.o
	
netServer.o : netServer.cpp
	$(CXX) $(CFLAGS) -c netServer.cpp -o netServer.o

netClient.o : netClient.cpp
	$(CXX) $(CFLAGS) -c netClient.cpp -o netClient.o
	
netClient.o_debug : netClient.cpp
	$(CXX) -D__Debug $(CFLAGS) -c netClient.cpp -o netClient.o
	
	
clean:
	rm netLib.a
	rm netServer.o
	rm netClient.o