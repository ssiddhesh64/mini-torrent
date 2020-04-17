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
    int socket_fd, val_read;
    struct sockaddr_in serv_addr;
    
    char buffer[1024] = {0};
    //char msg[1024];
    string msg;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
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

    close(socket_fd);

    return 0;
}
