#include<stdio.h>
#include<unistd.h>
#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string>

// #define port 8000
using namespace std;

int main(int argc, char** argv){

    // string p(argv[1]);
    // cout << p << endl;
    int port = stoi(argv[1]);
    int socket_fd, new_fd, val_read;
    struct sockaddr_in serv_address, cli_address;
    
    char buffer[1024] = {0};
    //char msg[1024];
    string msg;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&serv_address, '0', sizeof(serv_address));
    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(port);
    serv_address.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_fd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    
    if((listen(socket_fd, 5))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    cout << "listening" << endl;
    if((new_fd = accept(socket_fd, (struct sockaddr *) &cli_address, (socklen_t*)&cli_address)) < 0){
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    cout << "accepted connection" << endl;
    while(1){
        val_read = read(new_fd, buffer, 1024);
        if(strcmp(buffer, "bye")==0) break;
        printf("Client: %s\n", buffer);

        memset(buffer, 0, sizeof buffer); // Clears previous contents

        //msg.clear();     
        cout << "Server: ";
        getline(cin, msg);
        send(new_fd, msg.c_str(), msg.length(), 0);
            
    }
    close(socket_fd);
    return 0;
}
