
#include <stdint.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
        if(argc != 4){
                errx(1, "We need 3 arguments");
        }

        struct data{
                uint16_t offset;
                uint8_t b1;
                uint8_t b2;
        }__attribute__((packed));

        struct data patch;

        struct stat st1;
        if(stat(argv[1], &st1) < 0){
                err(2, "Error stat");
        }

        if(st1.st_size % sizeof(patch) != 0){
                errx(3, "Patch file not in format");
        }

        struct stat st2;
        if(stat(argv[2], &st2) < 0){
                err(2, "Error stat");
        }

        if(st2.st_size > 0xFFFF){
                warnx("Too big file. Possible mistakes");
        }

        int p = open(argv[1], O_RDONLY);
        if(p < 0){
                err(4, "Error openig");
        }

        int fd1 = open(argv[2], O_RDONLY);
        if(fd1 < 0){
                err(4, "Error opening");
        }

        int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR, S_IWUSR);
        if(fd2 < 0){
                err(4, "Error opening");
        }

        uint8_t buf;
        ssize_t bytes_read;

        while((bytes_read = read(fd1, &buf, sizeof(buf))) > 0){
                if(write(fd2, &buf, bytes_read) < 0){
                        err(6, "Error writing");
                }
        }

        if(bytes_read == -1){
                err(5, "Error readind");
        }

        while((bytes_read = read(p, &patch, sizeof(patch))) > 0){
                if(patch.offset > st2.st_size){
                        errx(7, "Incorrect offset, out of file. Offset: %d, size: %ld", patch.offset, st2.st_size);
                }

                if(lseek(fd2, patch.offset, SEEK_SET) < 0){
                        err(8, "Error lseek");
                }

                if(read(fd2,&buf, sizeof(buf)) < 0){
                        err(5, "Error reading");
                }

                if(buf != patch.b1){
                        errx(9, "Different patch file!");
                }

                if(lseek(fd2, patch.offset, SEEK_SET) < 0){
                        err(8, "Error lseek");
                }
	
                if(write(fd2, &patch.b2, sizeof(patch.b2)) < 0){
                        err(6, "Error writnig");
                }
        }

        if(bytes_read == -1){
                err(5, "Error readind");
        }

        close(fd1);
        close(fd2);
        close(p);

        exit(0);
}
