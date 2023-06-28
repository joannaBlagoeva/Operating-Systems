
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


int main(int argc, char* argv[]){

        if(argc != 3){
                errx(1, "We need 2 arguments");
        }

        if((strcmp(argv[1], "-d") != 0) && (strcmp(argv[1], "-s") != 0)){
                if(strlen(argv[1]) != strlen(argv[2])){
                        errx(2, "The first and second must have same length");
                }
        }

        ssize_t bytes_read;
        char buf;
        if(strcmp(argv[1], "-s") == 0){
                char prev = ' ';
                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        if((buf != prev && buf == argv[2][0]) || (buf != argv[2][0])){
                                if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                        err(4, "Error writing");
                                }

                                prev = buf;
                        }
                }

                if(bytes_read == -1){
                        err(3, "Error reading");
                }
        }
        else if(strcmp(argv[1], "-d") == 0){
                bool print = true;
                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        for(size_t i = 0; i < strlen(argv[2]); i++){
                                if(buf == argv[2][i]){
                                        print = false;
                                }
                        }

                        if(print){
                                if(write(1, &buf, bytes_read) != bytes_read){
                                        err(4, "Error writing");
                                }
                        }

                        print = true;
                }

                if(bytes_read == -1){
                        err(3, "Error reading");
                }
        }
        else{
                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        for(size_t i = 0; i < strlen(argv[1]); i++){
                                if(buf == argv[1][i]){
                                        buf = argv[2][i];
                                }
                        }

                        if(write(1, &buf, bytes_read) != bytes_read){
                                err(4, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(3, "Error reading");
                }
        }

        exit(0);
}