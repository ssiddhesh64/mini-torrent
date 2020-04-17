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
    struct sockaddr_in serv_addr;
    
    char buffer[1024] = {0};
    //char msg[1024];
    string msg;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
    while(1){
        cout << "Client: ";
        //cin.get(msg, 1024, '\n');
        //cin >> msg;
        getline(cin, msg);
        send(socket_fd, msg.c_str(), strlen(msg.c_str()), 0);
      //  printf("Message sent");
        if(strcmp(msg.c_str(), "bye")==0) break;
        val_read = read(socket_fd, buffer, 1024); 

     //   cout << "buffer length :" << strlen(buffer) << endl;
    //    cout << "read by client\n"; 
        printf("Server: %s\n", buffer);
    } 

    

    return 0;
}
