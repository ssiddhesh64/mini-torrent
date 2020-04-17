#include<stdio.h>
#include<unistd.h>
#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
//#include<string.h>
#include<string>
#define PORT 8000
using namespace std;

int main(){

    int socket_fd, new_fd, val_read;
    struct sockaddr_in address;
    
    char buffer[1024] = {0};
    //char msg[1024];
    string msg;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    
    if((listen(socket_fd, 5))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if((new_fd = accept(socket_fd, (struct sockaddr *) &address, (socklen_t*)&address)) < 0){
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    while(1){
        //cout << "accepted by server" << endl;
        val_read = read(new_fd, buffer, 1024);
        if(strcmp(buffer, "bye")==0) break;
        //cout << "read by serfer\n";
        printf("Client: %s\n", buffer);
        cout << "Server: ";
        //cin.get(msg, 1024, '\n');
        //cin >> msg;
        getline(cin, msg);
        send(new_fd, msg.c_str(), strlen(msg.c_str()), 0);
        

        printf("Message sent by server");
    
    }
    return 0;
}
