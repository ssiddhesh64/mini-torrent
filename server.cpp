/* This program takes 1 argument
    1. Port no
    run as ./server portno
*/
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string>
#include<string.h>
#include<thread> 
#include<arpa/inet.h>

#include"threadpool.h"
#include"error.h"
#include"client_info.h"

#define SERVER_BACKLOG 20
#define THREAD_POOL_SIZE 20

using namespace std;

int create_socket(int sock_fd, sockaddr_in &serv_address, int port){

    // create socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket opening failed");
        exit(EXIT_FAILURE);
    }

    // define connection parameters
    memset(&serv_address, '\0', sizeof(serv_address));
    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(port);
    serv_address.sin_addr.s_addr = INADDR_ANY;

    // bind socket to specific port
    check_error(bind(sock_fd, (struct sockaddr *) &serv_address, sizeof(serv_address)), "bind failed");
    
    // start listening on that port
    check_error(listen(sock_fd, SERVER_BACKLOG), "listen failed");
    cout << "listening on port: " << port << endl;

    return sock_fd;
}

int main(int argc, char** argv){

    int port = stoi(argv[1]);
    int sock_fd = 0, new_fd = 0;
    struct sockaddr_in serv_address, cli_address;
    
    // get socket descriptor
    sock_fd = create_socket(sock_fd, serv_address, port);

    Threadpool tp;
    // auto tp = Threadpool();    // Doesn't work as there is no copy constructor

    while(1){
        
        if((new_fd = accept(sock_fd, (struct sockaddr *) &cli_address, (socklen_t*)&cli_address)) < 0){
            perror("accept failed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        // client info
        client_info *cli = (client_info *)malloc(sizeof(client_info));
        cli->address = cli_address;
        cli->sock_fd = new_fd;
        
        cout << "connected successfully to client " << cli_address.sin_addr.s_addr << endl;
        tp.queue_work(cli);
    }
    
    close_connection(sock_fd);
    // close_connection(new_fd);
    return 0;
}


