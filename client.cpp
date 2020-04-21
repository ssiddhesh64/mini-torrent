/* This program takes 2 arguments
    1. Port Number of server which is running
    2. IP address of server which is running
    run as ./client portno ip_address
*/
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include<thread> 

#include"error.h"

using namespace std;

int create_socket(int &sock_fd,sockaddr_in &serv_addr, int port, char* ip){

    // create socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket opening failed");
        exit(EXIT_FAILURE);
    }

    // fill server details of server to be connected
    struct hostent *server;
    server = gethostbyname(ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        close(sock_fd);
        exit(0);
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // inet_pton(socket_fd, argv[2], &serv_addr.sin_addr.s_addr);
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    return sock_fd;
}

int main(int argc, char** argv){

    int port = stoi(argv[1]);
    int sock_fd, val_read;
    struct sockaddr_in serv_addr;
    
    char buffer[1024] = {0};
    memset(buffer, 0, sizeof buffer);   
    string msg;


    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket opening failed");
        exit(EXIT_FAILURE);
    }

    // fill server details of server to be connected
    struct hostent *server;
    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        close(sock_fd);
        exit(0);
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(sock_fd, argv[2], &serv_addr.sin_addr.s_addr);
    // bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // get socket descriptor
    // sock_fd = create_socket(sock_fd, serv_addr, port, argv[2]);

    // connect to server on particular IP and port
    check_error(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "couldn't connect to Server");
    cout << "successfully connected to server" << endl;

    while(1){       
        cout << "client: ";
        getline(cin, msg);
        check_error(send(sock_fd, msg.c_str(), msg.length(), 0), "send error");
        if(strcmp(msg.c_str(), "bye")==0) break;

        val_read = read(sock_fd, buffer, 1024); 
        if(val_read <= 0) break;

        printf("server: %s\n", buffer);
        memset(buffer, 0, sizeof buffer);               // Clears previous contents
    } 

    close_connection(sock_fd);

    return 0;
}
