#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#ifndef client_info_struct_h
#define client_info_struct_h

typedef struct {
    struct sockaddr_in address;
    int sock_fd;
    // int uid;
} client_info;


#endif