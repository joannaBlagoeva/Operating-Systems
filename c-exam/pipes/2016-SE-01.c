include <err.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


//cat file | sort
//   pfd[1]  pfd[0]
//pipe -------->

int main(int argc, char* argv[]){

        if(argc != 2){
                errx(1, "We need one argument");
        }

        int pfd[2];
        if(pipe(pfd) < 0){
                err(2, "Error pipe");
        }

        pid_t p = fork();

        if(p < 0){
                err(3, "Error fork");
        }
        else if(p == 0){
                close(pfd[0]); //close write
                if(dup2(pfd[1],1) < 0){ //replace stdout to out pipe
                        err(4, "Error dup2");
                }
                if(execlp("cat", "cat", argv[1], (char*)NULL) < 0){
                        err(5, "Error execlp");
                }
        }

        close(pfd[1]); // close read
        if(dup2(pfd[0], 0) < 0){
                err(4, "Error dup2");
        }
        if(execlp("sort", "sort", (char*)NULL) < 0){
                err(5, "Error execlp");
        }


        wait(NULL);
        exit(0);
}