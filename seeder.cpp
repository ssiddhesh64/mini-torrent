
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
#include<vector>
#include<math.h>
#include <sys/file.h>


#include"error.h"
#define BUFFER_SIZE 1024

#define SERVER_BACKLOG 20
#define THREAD_POOL_SIZE 20

using namespace std;

void process_request(char* reply, int &part_no, int &filesize, char** fullpath, int &part_size);
int upload(int sock_fd, int part_no, unsigned long size, char* fullpath, int part_size);

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
    char buffer[BUFFER_SIZE];
    
    // get socket descriptor
    sock_fd = create_socket(sock_fd, serv_address, port);

    // Threadpool tp;
    // auto tp = Threadpool();    // Doesn't work as there is no copy constructor

    while(1){
        
        if((new_fd = accept(sock_fd, (struct sockaddr *) &cli_address, (socklen_t*)&cli_address)) < 0){
            perror("accept failed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        // client info
        // client_info *cli = (client_info *)malloc(sizeof(client_info));
        // cli->address = cli_address;
        // cli->sock_fd = new_fd;
        
        cout << "connected successfully to client " << cli_address.sin_addr.s_addr << endl;

        int n_bytes = 0;
        memset(buffer, 0, BUFFER_SIZE);
        n_bytes = recv(new_fd, buffer, BUFFER_SIZE, 0);
        cout << "client: " << buffer << endl;
        cout << "bytes received: " << n_bytes << endl;
        char* fullpath;
        int part_no;
        int filesize;
        int part_size;
        process_request(buffer, part_no, filesize, &fullpath, part_size);

        thread th(upload, new_fd, part_no, filesize, fullpath, part_size);
        th.detach();
        // if(upload(new_fd, part_no, size) < 0){
        //     perror("error in uploading file");
        //     exit(EXIT_FAILURE);
        // }

        // tp.queue_work(cli);
        memset(buffer, 0, BUFFER_SIZE);
    }
    close(sock_fd);
    // close_connection(sock_fd);
    // close_connection(new_fd);
    return 0;
}

void process_request(char* reply, int &part_no, int &filesize, char** fullpath, int &part_size){

    vector<char*> tokens;
    char* token;
    token = strtok(reply, " ");
    while(token){
        tokens.push_back(token);
        token = strtok(NULL, " ");
    }
    *fullpath = realpath(tokens[0], NULL);
    cout << "requested file: " << *fullpath << endl;
    part_no = stoi(tokens[1]);
    filesize = stoi(tokens[2]);
    cout << filesize << " : filesize\n";
    part_size = stoi(tokens[3]);
    // cout << "n threads: " << n_threads << endl;
    // cout << fullpath << endl;
    // cout << part_no << endl;
    // cout << size << endl;
    cout << "exiting process request\n";
}


int upload(int sock_fd, int part_no, unsigned long size, char* fullpath, int part_size){

    // fullpath = realpath(fullpath, NULL);
    FILE* fp = fopen(fullpath, "rb");
    if(fp == NULL){
        perror("error opening file");
        printf("\nclosing connection\n");
        close(sock_fd);
        return -1;
    }

    // // Get file size 
    // fseek(fp, 0, SEEK_END);
    // unsigned long file_size = ftell(fp);
    // rewind(fp);  
    // cout << "filesize: " << file_size << endl;
    
    // int no_of_parts = ceil(float(file_size) / n_threads);                   // 512 * 1024 == 524288 bytes
    // int part_size = ceil(float(file_size) / n_threads);
    // int part_size = file_size / n_threads;
    // int rem_size = file_size % part_size;

    long remaining = size;
    int pos = part_no * part_size;                             // 512 * 1024 == 524288
    fseek(fp, pos, SEEK_SET);                         // point pos in beginning of file 
    
    int r_bytes = 0;
    char buffer[BUFFER_SIZE];
    int fd = fileno(fp);
    flock(fd, LOCK_SH);
    memset(buffer, 0, BUFFER_SIZE);
    while(remaining - BUFFER_SIZE >= 0){
        r_bytes = fread(buffer, 1, BUFFER_SIZE, fp);
        send(sock_fd, buffer, r_bytes, 0);
        cout << "sent " << r_bytes << " bytes\n";
        memset(buffer, 0, BUFFER_SIZE);
        remaining -= r_bytes;
    }
    memset(buffer, 0, BUFFER_SIZE);
    // send last remaining part
    r_bytes = fread(buffer, 1, remaining, fp);
    send(sock_fd, buffer, r_bytes, 0);

    printf("file sent successfully\n");
    flock(fd, LOCK_UN);
    fclose(fp);
    close(sock_fd);
    
    return 0;
}


