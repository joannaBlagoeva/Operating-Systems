#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char* argv[]){
        if (argc != 3){
                errx(1, "INvalid number of arguments");
        }

        int a[2];

        if (pipe(a) == -1){
                err(2, "Error while creating pipe");
        }

        pid_t pid;

        if ((pid = fork()) == -1){
                err(3, "Error while fork");
        }

        if (pid == 0){ // child
                close(a[0]);
                dup2(a[1],1);

                if (execlp("cat", "cat", argv[1],(char*)NULL) == -1){
                        err(4, "Error while execlp");
                }

        }

        close(a[1]);

        int o_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (o_fd == -1) {
                err(5, "Cannot create %s", argv[2]);
        }

        uint8_t c = 0;
        uint8_t t = 0;
        ssize_t rs = 0;
        uint8_t x = 0x20;


        while ((rs = read(a[0], &c, sizeof(c))) == sizeof(c)) {
                switch(c) {
                        case 0x55 :
                                continue;
                        case 0x7D :
                                if (read(a[0], &t, sizeof(t)) != sizeof(t)) {
                                        err(6, "Cannot read escaped byte");
                                }
                                if ((t != 0x20) && (t != 0xDF) &&
                                        (t != 0x75) && (t != 0x5D)) {
                                        errx(7, "Invalid escaped byte");
                                }
                                c = t^x;
                                dprintf("%x\n",c);

                                __attribute__ ((fallthrough));
                        default :
                                if (write(o_fd, &c, sizeof(c)) != sizeof(c)) {
                                        err(8, "Cannot write output byte");
                                }
                }
        }

        close(a[0]);
        close(o_fd);
        return 0;
}
