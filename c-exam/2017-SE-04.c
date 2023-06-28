#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]){
        if(argc == 1){
                char buf[4096];
                ssize_t bytes_read;
                while((bytes_read = read(0, &buf, sizeof(buf))) > 0){
                        if(write(1, &buf, bytes_read) != bytes_read){
                                err(2, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(1, "Error reading");
                }

                exit(0);
        }

        for(int i = 1;i < argc; i++){
                int fd;
                ssize_t bytes_read;
                if(strcmp(argv[i], "-") == 0){
                        fd = 0;
                }
                else{
                        fd = open(argv[i], O_RDONLY);
                        if(fd < 0){
                                warn("Error opening");
                                continue;
                        }
                }

                char buf[4096];

                while((bytes_read = read(fd, &buf, sizeof(buf))) > 0){
                        if(write(1,&buf, bytes_read) != bytes_read){
                                err(2, "Error writiing");
                        }
                }

                if(bytes_read == -1){
                        err(1, "Error reading");
                }
		
		if(fd != 0){
			close(fd);
		}
        }

        exit(0);
}