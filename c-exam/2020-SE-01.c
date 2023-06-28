#include <stdint.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]){

        if(argc != 7){
                errx(1, "We need 6 arguments");
        }

        struct header{
                char arr1[4];
                uint16_t count;
                char arr2[10];
        }__attribute__((packed));

        struct interval{
                uint16_t offset;
                uint16_t length;
        };

        struct interval k[4];

        int sizes[4];
        sizes[0] = 4;
        sizes[1] = 1;
        sizes[2] = 2;
        sizes[3] = 8;

        struct stat stAf;
        if(stat(argv[1], &stAf) < 0){
                err(2, "Error stat");
        }

        if(stAf.st_size < 16){
                errx(3, "Not enough for header");
        }

        int affix = open(argv[1], O_RDONLY);
        if(affix < 0){
                err(4, "Error opening");
        }

        struct header input;
        if(read(affix, &input, sizeof(input)) < 0){
                err(5, "Error reading");
        }

        if(input.count % 8 != 0){
                errx(6, "Wrong format affix file");
        }

        if((stAf.st_size - 16) % 16 != 0){
                errx(6, "Wrong format affix file");
        }

        //proverka ostanalite failove razmeri
        struct stat stPost;
        if(stat(argv[2], &stPost) < 0){
                err(2, "Error stat");
        }

        if(stPost.st_size < 16){
                errx(3, "Not enough for header");
        }

        if((stPost.st_size - 16) % sizeof(uint32_t) != 0){
                errx(7, "Wrong format file");
        }

        struct stat stPre;
        if(stat(argv[3], &stPre) < 0){
                err(2, "Error stat");
        }

        if(stPre.st_size < 16){
                errx(3, "Not enough for header");
        }

        struct stat stIn;
        if(stat(argv[4], &stIn) < 0){
                err(2, "Error stat");
        }

        if(stIn.st_size < 16){
                errx(3, "Not enough for header");
        }

        if((stIn.st_size - 16) % sizeof(uint16_t) != 0){
                errx(7, "Wrong format file");
        }

        //za sufix i crucifixus sushtoto


        int fd[4];

        for(int i = 2; i < (argc - 1); i++){
                fd[i-2] = open(argv[i], O_RDONLY);
                if(fd[i-2] < 0){
                        err(8, "Error opening");
                }
        }

        int fdCru = open(argv[6], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(fdCru < 0){
                err(8, "Error opening");
        }

        if(lseek(fdCru, 16, SEEK_SET) < 0){
                err(10, "Error lseek");
        }

        uint64_t element;
        ssize_t bytes_read;

        while((bytes_read = read(affix, &k, sizeof(k))) > 0){
                for(int i = 0; i < 4; i++){
                        if(lseek(fd[i], 16 + k[i].offset * sizes[i], SEEK_SET) < 0){
                                err(10, "Error lseek");
                        }
                        for(int j = 0; j < k[i].length; j++){
                                if(read(fd[i], &element, sizes[i]) < 0){
                                        err(9, "Error reading");
                                }

                                if(write(fdCru, &element, sizes[i]) != sizes[i]){
                                        err(11, "Error writing");
                                }
                        }
                }
        }

        if(bytes_read == -1){
                err(9, "Error reading");
        }

        close(fdCru);
        close(affix);

        for(int i = 0; i < 4; i++){
                close(fd[i]);
        }


        exit(0);
}