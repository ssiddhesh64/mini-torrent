/* This program takes 2 arguments
    1. Port Number of server which is running
    2. IP address of server which is running
    run as ./client portno ip_address
*/


#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string> 
#include<netdb.h>


using namespace std;

int main(int argc, char** argv){

    int port = stoi(argv[1]);
    int socket_fd, val_read;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024] = {0};
    //char msg[1024];
    string msg;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(socket_fd, argv[2], &serv_addr.sin_addr.s_addr);
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    cout << "IP address: " << serv_addr.sin_addr.s_addr << endl;

    if(connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connected" << endl;
    while(1){

        memset(buffer, 0, sizeof buffer);  // Clears previous contents

        //msg.clear();            // Clears previous contents
        cout << "Client: ";
        getline(cin, msg);
        send(socket_fd, msg.c_str(), msg.length(), 0);
        if(strcmp(msg.c_str(), "bye")==0) break;
        val_read = read(socket_fd, buffer, 1024); 

        printf("Server: %s\n", buffer);
    } 

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    return 0;
}
