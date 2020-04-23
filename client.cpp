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

#define BUFFER_SIZE 1024

#include"error.h"

using namespace std;

int create_socket(int sock_fd, sockaddr_in &serv_addr, int port, char* ip){

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
    inet_pton(sock_fd, ip, &serv_addr.sin_addr.s_addr);
    // bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    return sock_fd;
}

int fetch_file(int sock_fd){

    char buffer[BUFFER_SIZE];

    string file_name;
    cout << "Enter name of new file: ";
    cin >> file_name;

    cout << file_name << " file_name\n";
    FILE* fw = fopen(file_name.c_str(), "wb");
    if(fw == NULL){
        perror("error writing file\n");
        perror("closing connection to server");
        close(sock_fd);
        return -1;
    } 

    int n_bytes = 0;
    while((n_bytes = recv(sock_fd, buffer, BUFFER_SIZE, 0)) > 0){
        // printf("%s", buffer);
        fwrite(buffer, 1, n_bytes, fw);
    }
    fclose(fw);

    return 0;
}

int main(int argc, char** argv){

    int port = stoi(argv[1]);
    int sock_fd = 0;
    struct sockaddr_in serv_addr;
    
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof buffer);   
    string msg;

    sock_fd = create_socket(sock_fd, serv_addr, port, argv[2]);

    // connect to server on particular IP and port
    check_error(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "couldn't connect to Server");
    cout << "successfully connected to server..." << endl;

    cout << "enter filename to fetch: ";
    getline(cin, msg);
    check_error(write(sock_fd, msg.c_str(), msg.length()), "send error");
    if(strcmp(msg.c_str(), "bye")==0){
        perror("Connection closed");
        close(sock_fd);
        exit(EXIT_SUCCESS);
    }

    printf("reading from server\n");

    if(fetch_file(sock_fd) < 0) return EXIT_FAILURE;
    
    printf("\nserver's response completed\n");
    printf("closing connection to server\n");
    fflush(stdout);

    close(sock_fd);

    return 0;
}
