#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#ifndef client_info_h
#define client_info_h


// client info structure
typedef struct {
    struct sockaddr_in address;
    int sock_fd;
} client_info;

#endif