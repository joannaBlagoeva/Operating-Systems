#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]){
        if(argc < 3){
                errx(1, "We need at least 2 arguments");
        }

        if(strcmp(argv[1], "-c") == 0){
                int n, m;
                if(strlen(argv[2]) == 1){
                        n = argv[2][0] - '0';
                        m = n;
                }
                else if(strlen(argv[2]) == 3){
                        n = argv[2][0] - '0';
                        m = argv[2][2] - '0';

                        if(n > m){
                                errx(3, "The second must be bigger");
                        }
                }
                else{
                        errx(4, "Wrong format of argument");
                }

                ssize_t bytes_read;
                ssize_t cnt = 0;
                char buf;

                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        cnt += sizeof(buf);

                        if(buf == '\n'){
                                cnt = 0;
                                if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                        err(6, "Error writing");
                                }

                                continue;
                        }

                        if(cnt >=n && cnt <= m){
                                if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                        err(6, "Error writing");
                                }
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }
        }
        else if(strcmp(argv[1], "-d") == 0){
                if(argc != 5){
                        errx(7, "Not enough arguments for -d");
                }

                if(strcmp(argv[3], "-f") != 0){
                        errx(8, "Wrong format for -d");
                }

                char delim = argv[2][0];
                int n, m;
                if(strlen(argv[4]) == 1){
                        n = argv[4][0] - '0';
                        m = n;
                }
                else if(strlen(argv[4]) == 3){
                        n = argv[4][0] - '0';
                        m = argv[4][2] - '0';

                        if(n > m){
                                errx(3, "The second must be bigger");
                        }

                }
                else{
                        errx(4, "Wrong format of argument");
                }

                int col = 1;
                char buf;
                ssize_t bytes_read;

                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        if(buf == delim){
                                col++;

                                if(col > n && col <= m){
                                        if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                                err(6, "Error writing");
                                        }
                                }
                        }
                        else if(buf == '\n'){
                                col = 1;
                                if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                        err(6, "Error writing");
                                }
                        }
                        else{
                                if(col >= n && col <= m){
                                        if(write(1, &buf, sizeof(buf)) != sizeof(buf)){
                                                err(6, "Error writing");
                                        }
                                }
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }

        }
        else{
                errx(2, "Not in correct format");
        }

        exit(0);
}