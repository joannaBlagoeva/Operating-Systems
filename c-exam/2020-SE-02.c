#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[]){
        if(argc != 3){
                errx(1, "We need 2 arguments");
        }

        struct stat st;
        if(stat(argv[2], &st) < 0){
                err(2, "Error stat");
        }

        if(st.st_size % 16 != 0){
                errx(3, "Wrong format file");
        }

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

        uint16_t input[8];

        int fd1 = open(argv[1], O_RDONLY);
        if(fd1 < 0){
                err(4, "Error opening");
        }

        int fd2 = open(argv[2], O_RDONLY);
        if(fd2 < 0){
                err(4, "Error opening");
        }

        int fd3 = open("file.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(fd3 < 0){
                err(4, "Error opening");
        }

        uint8_t bufBit;

        //proverki trqbva da ima
        while(read(fd1, &bufBit, sizeof(bufBit)) > 0){

                read(fd2, &input, sizeof(input));
                for(int i = 7; i >= 0; i--){
                        if((bufBit & one[i]) != 0){
                                write(fd3, &input[7-i], sizeof(uint16_t));
                        }
                }
        }


        exit(0);
}
