
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

int comparator(const void* p1, const void* p2){
        uint32_t n1 = *(uint32_t*) p1;
        uint32_t n2 = *(uint32_t*) p2;

        return n1 - n2;
}

int main(int argc, char* argv[]){
        if(argc != 3){
                errx(1, "We need 2 argument");
        }

        struct stat st;
        if(stat(argv[1], &st) < 0){
                err(2, "Error stat");
        }

        if(st.st_size % sizeof(uint32_t) != 0){
                errx(3, "Not correct size");
        }

        uint32_t numbers = st.st_size / sizeof(uint32_t);
        const uint32_t maxN = 4194304;
        if(numbers > maxN){
                errx(4, "Too many numbers");
        }

        uint32_t first = numbers / 2;
        uint32_t second = numbers - first;

        int fd1 = open(argv[1], O_RDONLY);
        if(fd1 < 0){
                err(5, "Error opening");
        }

        uint32_t* buf = malloc(first * sizeof(uint32_t));

        if(!buf){
                err(6, "Error malloc");
        }

        if(read(fd1, buf, first * sizeof(uint32_t)) < 0){
                err(7, "Error reading");
        }

        int temp1 = open("temp1", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(temp1 < 0){
                err(5, "Error opening");
        }

        qsort(buf, first, sizeof(uint32_t), comparator);

        if(write(temp1, buf, first * sizeof(uint32_t)) < 0){
                err(8, "Error writing");
        }

        free(buf);

        buf = malloc(second * sizeof(uint32_t));

        if(!buf){
                err(6, "Error malloc");
        }

        if(read(fd1, buf, first * sizeof(uint32_t)) < 0){
                err(7, "Error reading");
        }

        int temp2 = open("temp2", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR, S_IWUSR);
        if(temp2 < 0){
                err(5, "Error opening");
        }

        qsort(buf, second, sizeof(uint32_t), comparator);

        if(write(temp2, buf, sizeof(uint32_t)) < 0){
                err(8, "Error writing");
        }

        free(buf);

        int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
        if(fd2 < 0){
                err(5, "Error opening");
        }

        uint32_t cnt1 = 0;
        uint32_t cnt2 = 0;

        uint32_t num1;
        uint32_t num2;

        while(cnt1 < first && cnt2 < second){
                if(lseek(temp1, cnt1 * sizeof(uint32_t), SEEK_SET) < 0){
                        err(9,"Error lseek");
                }

                if(lseek(temp2, cnt2 * sizeof(uint32_t), SEEK_SET) < 0){
                        err(9, "Error lseek");
                }

                if(read(temp1, &num1, sizeof(uint32_t)) < 0){
                        err(7, "Error reading");
                }

                if(read(temp2, &num2, sizeof(uint32_t)) < 0){
                        err(7, "Error reading");
                }

                if(comparator(&num1, &num2) < 0){
                        if(write(fd2, &num1, sizeof(num1)) < 0){
                                err(8, "Error writing");
                        }

                        cnt1++;
                }
                else{
                        if(write(fd2, &num2, sizeof(num2)) < 0){
                                err(8, "Error writing");
                        }

                        cnt2++;
                }
        }

        while(cnt1 < first){
                if(lseek(temp1, cnt1*sizeof(uint32_t), SEEK_SET) < 0){
                        err(9, "Error lseek");
                }

                if(read(temp1, &num1, sizeof(num1)) < 0){
                        err(7, "Error reading");
                }

                if(write(fd2, &num1, sizeof(num1)) < 0){
                        err(8, "Error writing");
                }

                cnt1++;
        }

        while(cnt2 < second){
                if(lseek(temp2, cnt2*sizeof(uint32_t), SEEK_SET) < 0){
                        err(9, "Error lseek");
                }

                if(read(temp2, &num2, sizeof(num2)) < 0){
                        err(7, "Error reading");
                }

                if(write(fd2, &num2, sizeof(num2)) < 0){
                        err(8, "Error writing");
                }

                cnt2++;
        }

        close(fd2);
        close(temp1);
        close(temp2);
        close(fd1);

        exit(0);
}
