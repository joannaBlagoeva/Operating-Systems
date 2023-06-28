
#include <err.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>



// cut -d ':' -f 7 /etc/passwd | sort | uniq -c | sort -n -k 1,1
//                          ------>  ------>  ----->
//                        a[1] a[0] b[1] b[0] c[1] c[0]

int main(void){
        int a[2];
        if(pipe(a) < 0){
                err(1, "Error pipe");
        }

        pid_t p = fork();
        if(p < 0){
                err(2, "Error fork");
        }
        else if(p == 0){
                close(a[0]);
                if(dup2(a[1],1) < 0){
                        err(3, "Error dup2");
                }
                if(execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL) < 0){
                        err(4,"Error execlp");
                }
        }

        close(a[1]);

        int b[2];
        if(pipe(b) < 0){
                err(1, "Error pipe");
        }

        pid_t q = fork();
        if(q < 0){
                err(2, "Error fork");
        }
        else if(q == 0){
                close(b[0]);
                if(dup2(a[0],0) < 0){
                        err(3, "Error dup2");
                }

                if(dup2(b[1],1) < 0){
                        err(3, "Error dup2");
                }

                if(execlp("sort", "sort", (char*)NULL) < 0){
                        err(4, "Error execlp");
                }
        }

        close(b[1]);
        close(a[0]);

        int c[2];

        if(pipe(c) < 0){
                err(1, "Error pipe");
        }

        pid_t r = fork();

        if(r < 0){
                err(2,"Error fork");
        }
        else if(r == 0){
                close(c[0]);

                if(dup2(b[0],0) < 0){
                        err(3, "Error dup2");
                }

                if(dup2(c[1],1) < 0){
                        err(3, "Error dup2");
                }

                if(execlp("uniq", "uniq", "-c", (char*)NULL) < 0){
                        err(4, "Error execlp");
                }
        }

        close(b[0]);
        close(c[1]);

        if(dup2(c[0], 0) < 0){
                err(3, "Error dup2");
        }

        if(execlp("sort", "sort", "-n", "-k1,1", (char*)NULL) < 0){
                err(4, "Error execlp");
        }


        exit(0);
}
