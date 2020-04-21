CC=g++-9

CFLAGS= -c -Wall
LIBS=-pthread
BINS= server client

all: $(BINS)

server: server.o threadpool.h client_info.h error.h
	$(CC) -o $@ $^ $(LIBS)

client: client.o error.h
	$(CC) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $^ -std=c++0x -o $@  

.PHONE: clean

clean:
	rm -r *.o client server
	clear