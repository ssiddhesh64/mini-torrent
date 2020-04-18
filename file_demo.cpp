/*
 This program takes 2 arguments 
    1. File name which is to be copied
    2. Name of copied file
*/

#include<bits/stdc++.h>

using namespace std;
string home = "/home/sid/Desktop/";                         // ~/Desktop doesn't work


int copy_file(FILE* from, FILE* To);

int main(int argc, char** argv){

    char* file_path = argv[1];
    FILE* f;                                                // file pointer

    // Open file
    f = fopen(argv[1], "rb");
    if(!f){
        perror("Error in opening file");
        exit(EXIT_FAILURE);
    }
    
    string temp = argv[2];
    string dest = home + temp;

    // Open a file to write the copied contents 
    FILE* fw = fopen(dest.c_str(), "wb");
   
    if(copy_file(f, fw) != 0){
        perror("Error in copying file");
    }
    fclose(f);     
    fclose(fw);
    return 0;

}

int copy_file(FILE* from, FILE* To){

    // Create temporary buffer to store contents of file which we are copying
    char buffer[1024];
    memset(buffer, 0, 1024);


    // Get file size 
    fseek(from, 0, SEEK_END);
    long file_size = ftell(from);
    rewind(from);                                           // Get pointer to start position

    // We copy in parts of 512 kilo-bytes, so calculate no. of parts and size of last remaining part
    int no_of_parts = file_size / 524288;                   // 512 * 1024 == 524288 bytes
    int rem_size = file_size % 524288;

    // Write parts in size of 512 kilo-bytes
    for(int i = 0; i < no_of_parts - 1; i++){
        int j = 0;
        int pos = i * 524288;                               // 512 * 1024 == 524288
        fseek(from, pos, SEEK_SET);                         // point pos in beginning of file 
        while(j < 512){
            int result = fread(buffer, 1, 1024, from);
            fwrite(buffer, sizeof(char), result, To);
            j++;
        }
    }

    // Write last remaining part 
    int j = 0;
    fseek(from, (no_of_parts - 1) * 524288, SEEK_SET); 
    while(j < rem_size){
        int result = fread(buffer, 1, 1024, from);
        fwrite(buffer, sizeof(char), result, To);
        j++;
    }

    // Closing files
    fclose(To);
    fclose(from);
    return 0;
}