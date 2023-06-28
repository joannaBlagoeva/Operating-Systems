#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


int main(int argc, char* argv[]){
        if(argc != 4){
                errx(1, "We need 3 arguments");
        }

        struct header{
                uint32_t magic;
                uint8_t header_version;
                uint8_t data_version;
                uint16_t count;
                uint32_t res1;
                uint32_t res2;
        }__attribute__((packed));

        struct header input;

        struct data1{
                uint16_t offset;
                uint8_t original;
                uint8_t new;
        }__attribute__((packed));

        struct data1 part1;

        struct data2{
                uint32_t offset;
                uint16_t original;
                uint16_t new;
        }__attribute__((packed));

        struct data2 part2;

        struct stat st;
        if(stat(argv[1], &st) < 0){
                err(2, "Error stat");
        }

        if(st.st_size < 0x10){
                errx(3, "We dont have enough for header");
        }

        int pt = open(argv[1], O_RDONLY);
        if(pt < 0){
                err(4, "Error opening");
        }

        if(read(pt, &input, sizeof(input)) < 0){
                err(5, "Error reading");
        }

        if(input.magic != 0xEFBEADDE){
                errx(6, "Incorrect header data");
        }

        if(input.header_version != 0x01){
                errx(6, "Incorrect header data");
        }

        if(input.data_version == 0x00){
                struct stat st1;
                if(stat(argv[2], &st1) < 0){
                        err(2, "Error stat");
                }

                if(st1.st_size > 0xFFFF){
                        warnx("Maybe have mistakes");
                }

                if((size_t)st.st_size != (sizeof(input) + input.count * sizeof(uint32_t))){
                        errx(11, "Wrong format file");
                }

                int fd1 = open(argv[2], O_RDONLY);
                if(fd1 < 0){
                        err(4, "Error opening");
                }

                int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
                if(fd2 < 0){
                        err(4, "Error opening");
                }

                uint8_t buf;
                ssize_t bytes_read;
                while((bytes_read = read(fd1, &buf, sizeof(buf))) > 0){
                        if(write(fd2, &buf, sizeof(buf)) != sizeof(buf)){
                                err(6, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }

                while((bytes_read = read(pt, &part1, sizeof(part1))) > 0){
                        if(part1.offset > st1.st_size){
                                errx(8, "Incorrect offset");
                        }

                        if(lseek(fd1, part1.offset, SEEK_SET) < 0){
                                err(9, "Error lseek");
                        }

                        if(read(fd1, &buf, sizeof(buf)) < 0){
                                err(5, "Error reading");
                        }

                        if(buf != part1.original){
                                errx(10, "Wrong patch file");
                        }

                        if(lseek(fd2, part1.offset, SEEK_SET) < 0){
                                err(9, "Error lseek");
                        }

                        if(write(fd2, &part1.new, sizeof(uint8_t)) != sizeof(uint8_t)){
                                err(6, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }

                close(fd1);
                close(fd2);
        }
        else if(input.data_version == 0x01){
                struct stat st2;
                if(stat(argv[2], &st2) < 0){
                        err(2, "Error stat");
                }

                if(st2.st_size > 0xFFFFFFFF){
                        warnx("Maybe have mistakes");
                }

                if(st2.st_size % 2 != 0){
                        errx(7, "Wrong format file");
                }

                if((size_t)st.st_size != (sizeof(input) + input.count * sizeof(part2))){
                        errx(11,"Wrong patch file");
                }

                int fd1 = open(argv[2], O_RDONLY);
                if(fd1 < 0){
                        err(4, "Error opening");
                }

                int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
                if(fd2 < 0){
                        err(4, "Error opening");
                }

                uint16_t buf;
                ssize_t bytes_read;
                while((bytes_read = read(fd1, &buf, sizeof(buf))) > 0){
                        if(write(fd2, &buf, sizeof(buf)) != sizeof(buf)){
                                err(6, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }

                while((bytes_read = read(pt, &part2, sizeof(part2))) > 0){
                        if(part2.offset > st2.st_size){
                                errx(8, "Incorrect offset");
                        }

                        if(lseek(fd1, part2.offset * sizeof(uint16_t), SEEK_SET) < 0){
                                err(9, "Error lseek");
                        }

                        if(read(fd1, &buf, sizeof(buf)) < 0){
                                err(5, "Error reading");
                        }

                        if(buf != part2.original){
                                errx(10, "Wrong patch file");
                        }

                        if(lseek(fd2, part2.offset * sizeof(uint16_t), SEEK_SET) < 0){
                                err(9, "Error lseek");
                        }

                        if(write(fd2, &part2.new, sizeof(uint16_t)) != sizeof(uint16_t)){
                                err(6, "Error writing");
                        }
                }

                if(bytes_read == -1){
                        err(5, "Error reading");
                }

                close(fd1);
                close(fd2);

        }
        else{
                errx(12, "Wrong data version");
        }

        close(pt);

        exit(0);
}