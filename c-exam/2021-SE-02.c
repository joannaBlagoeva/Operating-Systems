#include <err.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

uint16_t calc(uint8_t);

int main(int argc, char* argv[]){
        if(argc != 3){
                errx(1, "We need 2 arguments");
        }

        int fd1 = open(argv[1], O_RDONLY);
        if(fd1 < 0){
                err(2, "Error opening");
        }

        int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(fd2 < 0){
                err(2, "Error opening");
        }

        ssize_t bytes_read;
        uint8_t buf;
        uint16_t encode;

        while((bytes_read = read(fd1, &buf, sizeof(uint8_t))) > 0){
                encode = calc(buf);
                if(write(fd2, &encode, sizeof(uint16_t)) != sizeof(uint16_t)){
                        err(4, "Error writing");
                }
        }

        if(bytes_read == -1){
                err(3, "Error reading");
        }

        close(fd1);
        close(fd2);

        exit(0);
}

uint16_t calc(uint8_t x){
        uint8_t one[8] = {
                0x01,
                0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
                0x80,
        };

/*
0x01 (binary: 00000001)
0x02 (binary: 00000010)
0x04 (binary: 00000100)
0x08 (binary: 00001000)
0x10 (binary: 00010000)
0x20 (binary: 00100000)
0x40 (binary: 01000000)
0x80 (binary: 10000000)
*/

        uint16_t two[8] = {
        0x0002,
            0x0008,
        0x0020,
            0x0080,
        0x0200,
            0x0800,
            0x2000,
            0x8000,
        };

        uint16_t three[8] = {
        0x0001,
            0x0004,
        0x0010,
            0x0040,
        0x0100,
            0x0400,
            0x1000,
            0x4000,
        };

        uint16_t result = 0;
        for(int i = 0; i < 8; i++){
                if((x & one[i]) != 0){
                        result = result | two[i];
                }
                else{
                        result = result | three[i];
                }
        }

        return result;
}
