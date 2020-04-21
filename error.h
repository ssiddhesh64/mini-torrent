#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>

int check_error(int ret_val, const char* msg){

    if(ret_val < 0){
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return ret_val;
}

int close_connection(int sock_fd){

    check_error(shutdown(sock_fd, SHUT_RDWR), "error in socket shutdown");
    check_error(close(sock_fd), "error closing socket");

    return 0;
}