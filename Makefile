CC=g++

CFLAGS= -c -Wall
LIBS=-pthread
BINS= server client

all: $(BINS)

client: client.o error.o
	$(CC) -o $@ $^ $(LIBS)

server: server.o error.o
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@  

.PHONE: clean

clean:
	rm -r *.o client server
	clear