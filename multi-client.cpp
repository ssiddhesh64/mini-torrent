
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
#include<math.h>
#include <sys/file.h>


#include <cstddef>         // std::size_t

#define BUFFER_SIZE 1024

#include"error.h"

using namespace std;

string get_file_name(const string& path);

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

void download(string filename, int part_no, int size, const string &path, int port, int part_size){

    int sock_fd = 0;
    struct sockaddr_in serv_addr;
    sock_fd = create_socket(sock_fd, serv_addr, port, "127.0.0.1");
    check_error(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "couldn't connect to Server");
    cout << "successfully connected to server..." << endl;

    string query = path + " " + to_string(part_no) + " " + to_string(size) + " " + to_string(part_size);
    cout << "query: " << query << endl;
    check_error(write(sock_fd, query.c_str(), query.length()), "send error");
    // if(strcmp(query.c_str(), "bye")==0){
    //     perror("Connection closed");
    //     close(sock_fd);
    //     exit(EXIT_SUCCESS);
    // }
    FILE* fw = fopen(filename.c_str(), "wb");
    if(fw == NULL){
        perror("error opening file\n");
        perror("closing connection to server");
        return ;
    } 

    int pos = part_no * part_size;                               // 512 * 1024 == 524288
    fseek(fw, pos, SEEK_SET);

    int n_bytes = 0;
    char buffer[BUFFER_SIZE];
    // int fd = fileno(fw);
    // flock(fd, LOCK_EX);
    memset(buffer, 0, BUFFER_SIZE);
    while((n_bytes = recv(sock_fd, buffer, BUFFER_SIZE, 0)) > 0){
        // printf("%s", buffer);
        fwrite(buffer, 1, n_bytes, fw);
        memset(buffer, 0, BUFFER_SIZE);
    }
    // flock(fd, LOCK_UN);
    fclose(fw);
    close(sock_fd);
}

int fetch_file(const string& path, int port){

    // char buffer[BUFFER_SIZE];
    string download_dir = "./Downloads";
    char* fullpath = realpath(download_dir.c_str(), NULL);
    download_dir = fullpath;
    char* ff = realpath(path.c_str(), NULL);
    string filename = get_file_name(ff);
    FILE* fp = fopen(ff, "rb");
    // Get file size 
    fseek(fp, 0, SEEK_END);
    unsigned long file_size = ftell(fp);
    rewind(fp);  
    fclose(fp);
    cout << "filesize: " << file_size << endl;
    filename = download_dir + "/" + filename;

    cout << "file to be created: " << filename << endl;
    // FILE* fw = fopen(filename.c_str(), "ab");
    // if(fw == NULL){
    //     perror("error writing file\n");
    //     perror("closing connection to server");
    //     return -1;
    // } 

    // cout << "opened file successfully\n";
    //--------------------------------------------------------------------
    // create threads to download file simultaneaously
    // We download in parts of 512 kilo-bytes, so calculate no. of parts and size of last remaining part
    int n_threads = 3;
    
    // int no_of_parts = ceil(float(file_size) / n_threads);                   // 512 * 1024 == 524288 bytes
    // int part_size = ceil(float(file_size) / n_threads);
    int part_size = file_size / n_threads;
    int rem_size = file_size % part_size + part_size;

    
    cout << "number of threads: " << n_threads << endl;
    thread download_thread[n_threads];

    cout << "starting creating threads\n";
    for(int j = 0; j < n_threads; j++){
        if(j == n_threads - 1)
            download_thread[j] = thread(download, filename, j, rem_size, path, port, part_size);
        else
            download_thread[j] = thread(download, filename, j, part_size, path, port, part_size);
        // download_thread[j].join();
    }
    for(int j = 0; j < n_threads; j++){
        download_thread[j].join();
    }
    cout << "thread download complete\n";


    //-----------------------------------------------------------

    
    // string file_name;
    // cout << "Enter name of new file: ";
    // cin >> file_name;

    // cout << filename << " file_name\n";
    // FILE* fw = fopen(filename.c_str(), "wb");
    // FILE* fw = fopen("black.mp4", "wb");
    

    // int n_bytes = 0;
    // while((n_bytes = recv(sock_fd, buffer, BUFFER_SIZE, 0)) > 0){
    //     // printf("%s", buffer);
    //     fwrite(buffer, 1, n_bytes, fw);
    // }
    // fclose(fw);

    return 0;
}

string get_file_name(const string& path){

    std::cout << "Splitting: " << path << '\n';
    std::size_t found = path.find_last_of("/\\");
    cout << "/ found at pos: " << found << endl;
    // cout << "filename: " << filename << endl;
    if(found == -1) return path;
    cout << "should not print\n";
    string filename = path.substr(found+1);
    return filename;
}

int main(int argc, char** argv){

    int port = stoi(argv[1]);
    // int sock_fd = 0;
    // struct sockaddr_in serv_addr;
    
    // char buffer[BUFFER_SIZE];
    // memset(buffer, 0, sizeof buffer);   
    // string msg;

    // sock_fd = create_socket(sock_fd, serv_addr, port, argv[2]);

    // // connect to server on particular IP and port
    // check_error(connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "couldn't connect to Server");
    // cout << "successfully connected to server..." << endl;

    string path;
    // int part_no;
    // int size;
    cout << "enter filename with path to fetch: ";
    cin >> path;
    // cout << "enter part no to fetch: ";
    // cin >> part_no;
    // cout << "enter size to fetch: ";
    // cin >> size;

    // string query = path + to_string(part_no) + to_string(size);

    if(fetch_file(path, port) < 0) return EXIT_FAILURE;
    
    printf("\nserver's response completed\n");
    printf("closing connection to server\n");
    fflush(stdout);

    return 0;
}
