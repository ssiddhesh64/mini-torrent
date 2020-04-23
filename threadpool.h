#include<iostream>
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>

#include <limits.h>     // for realpath
#include <stdlib.h>     // for realpath

#include<thread>
#include<queue>
#include<vector>
#include<mutex>
#include <condition_variable>
#include"client_info.h"

#define BUFFER_SIZE 1024

// Threadpool class for server concurrency

// void process_request(client_info* cli);
void process_request_get_file(client_info* cli);

using namespace std;

class Threadpool{
    public:
        Threadpool() : done(false){

            // initialize number of threads as number of cores for simplicity
            unsigned int n_threads = thread :: hardware_concurrency();
            // if function can't figure out, it returns 0
            // hence n_threads should atleast be 1;
            if(n_threads == 0){
                n_threads = 1;
            }

            // create threads vector and 
            for(unsigned int i = 0; i < n_threads; i++){
                threads.push_back(thread(&Threadpool::do_work, this));
            }
        }

        // destructor
        ~Threadpool(){

            // so threads know when to shutdown
            done = true;

            // wake all threads to finish and be joined
            work_queue_cond_var.notify_all();
            for(auto& thread : threads){
                if(thread.joinable())
                    thread.join();
            }
        }

        void queue_work(client_info* cli){
            // Grab the mutex
            std::lock_guard<std::mutex> g(work_queue_mutex);

            // Push the request to the queue
            work_queue.push(cli);

            // Notify one thread that there are requests to process
            work_queue_cond_var.notify_one();
        }

    private:
        vector<thread> threads; 
        queue<client_info*> work_queue; 
        bool done;
        condition_variable_any work_queue_cond_var;
        mutex work_queue_mutex;

        void do_work(){

            while(!done){   

                client_info* cli;
                // Create a scope, so we don't lock the queue for longer than necessary
                {
                    unique_lock<mutex> g(work_queue_mutex);
                    work_queue_cond_var.wait(g, [&]{
                        // Only wake up if there are elements in the queue or the program is
                        // shutting down
                        return !work_queue.empty() || done;
                    });

                    cli = work_queue.front();
                    work_queue.pop();
                }
                // process_request(cli);
                process_request_get_file(cli);   
            }
        }    
};

int send_file(char* fullpath, char* buffer, int sock_fd){

    memset(buffer, 0, BUFFER_SIZE); 
    FILE* fp = fopen(fullpath, "r");
    if(fp == NULL){
        perror("error opening file");
        printf("\nclosing connection\n");
        close(sock_fd);
        return -1;
    }

    long n_bytes = 0;
    memset(buffer, 0, BUFFER_SIZE);   
    while((n_bytes = fread(buffer, 1, 1024, fp)) > 0){
        printf("sending %zu bytes\n", (unsigned long)n_bytes);
        send(sock_fd, buffer, n_bytes, 0);
    }
    fclose(fp);
    printf("file sent successfully\n");

    return 0;
}

void process_request_get_file(client_info* cli) {

    int new_fd = cli->sock_fd;
    int n_bytes = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof buffer);   

    n_bytes = read(cli->sock_fd, buffer, BUFFER_SIZE);
    if(n_bytes < 0){
        perror("read error");
        close(new_fd);
        exit(EXIT_FAILURE);
    }
    // NULL terminate buffer
    buffer[n_bytes] = 0;

    char* fullpath = realpath(buffer, NULL);
    if(send_file(fullpath, buffer, new_fd) < 0){
        perror("error in sending file");
        exit(EXIT_FAILURE);
    }

    // close client socket
    printf("request completed, closing connection\n");
    close(new_fd);
}

// void process_request(client_info* cli) {

//     int new_fd = cli->sock_fd;
//     int n_bytes = 0;
//     char buffer[1024];
//     memset(buffer, 0, sizeof buffer);   
//     string msg;

//     cout << "getting in while loop" << endl;
//     while(1){   
//         cout << new_fd << " sock_fd" << endl;
//         n_bytes = read(cli->sock_fd, buffer, 1024);
//         if(n_bytes < 0){
//             perror("read error");
//             break;
//         }
//         printf("client: %s\n", buffer);
//         fflush(stdout);
//         if(strcmp(buffer, "bye")==0){
//             char ip[30];
//             inet_ntop(AF_INET, &cli->address.sin_addr, ip, sizeof(ip));
//             cout << "client " << ip << " disconnected\n";
//             break;
//         }
//         memset(buffer, 0, sizeof buffer);               // Clears previous buffer contents

//         cout << "server: ";
//         getline(cin, msg);
//         send(new_fd, msg.c_str(), msg.length(), 0);
//     }
//     // close client socket
//     close(new_fd);
    
// }

