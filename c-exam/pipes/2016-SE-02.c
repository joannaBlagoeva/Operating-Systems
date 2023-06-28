
#include <err.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


int main(void){

        while(true){
                char buf[1024];
                ssize_t bytes_read;

                if((bytes_read = read(0, &buf, sizeof(buf))) < 0){
                        err(1, "Error reading");
                }

                buf[bytes_read - 1] = '\0';

                if(strcmp(buf, "exit") == 0){
                        break;
                }

                pid_t p = fork();

                if(p < 0){
                        warn("Error fork");
                        continue;
                }
                else if(p == 0){
                        if(execlp(buf, buf, (char*)NULL) < 0){
                                warn("Error execpl");
                                continue;
                        }
                }

                wait(NULL);
        }

        exit(0);
}