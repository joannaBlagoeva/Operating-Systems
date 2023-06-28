#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>

int main(int argc, char* argv[]){

        if(argc != 3){
                err(1,"Incorrect number of arguments");
        }

        struct data_header{
                uint32_t magic;
                uint32_t count;
        }__attribute__((packed));

        struct comp_header{
                uint32_t magic1;
                uint32_t magic2;
                uint16_t reserved; //no use
                uint64_t count;
        }__attribute__((packed));

        struct comp_data{
                uint16_t type;
                uint16_t reserved1;
                uint16_t reserved2;
                uint16_t reserved3;
                uint32_t offset1;
                uint32_t offset2;
        }__attribute__((packed));

        int fd1,fd2;
        struct data_header dh;

        //work on fd1
        if((fd1=open(argv[1],O_RDWR)) == -1){
                err(2,"Error on open");
        }

        struct stat st1;
        if(stat(argv[1],&st1) == -1){
                err(3,"Error on stat");
        }


        if(read(fd1,&dh,sizeof(dh)) == -1){
                err(4,"Error on read");
        }

        if( (st1.st_size - sizeof(dh)) % sizeof(uint64_t) != 0 ){
                err(5,"Wrong format of f1 data");
        }

        if ((st1.st_size-sizeof(dh)) % dh.count != 0){
                err(5,"Wrong size of f1");
        }

        if( dh.magic != 0x21796F4A){
                err(5,"Wrong format of f1");
        }

        //work on fd2
        if((fd2=open(argv[2],O_RDONLY)) == -1){
                err(2,"Error on open");
        }
        struct comp_header ch;
        struct comp_data cd;

        struct stat st2;
        if(stat(argv[2],&st2) == -1){
                err(3,"Error on stat");
        }

        if(read(fd2,&ch,sizeof(ch)) == -1){
                err(4,"Error on read");
        }

        if(ch.magic1 != 0xAFBC7A37){
                err(5,"Wrong format of f2");
        }
        if(ch.magic2 != 0x1C27){
                err(5,"Wrong format of f2");
        }

        if((st2.st_size-sizeof(ch)) % sizeof(cd) !=0){
                err(5,"Wrong format of f2");
        }

        if(st2.st_size-sizeof(ch)%ch.count !=0){
                err(5,"Wrong format of f2");
        }

        //done with checks on f1 & f2

        ssize_t bytes_read;
        uint64_t el1;
        uint64_t el2;

        while((bytes_read=read(fd2,&cd,sizeof(cd)))>0){

                if(cd.type !=0 && cd.type != 1){
                        err(6,"Error incorrect type");
                }

                if(cd.reserved1 != 0 || cd.reserved2!=0 || cd.reserved3!=0){
                        err(6,"Error invalid comparator data");
                }

                //0


                if(cd.offset1 > ch.count || cd.offset2 > ch.count){
                        err(6,"No such address in f1");
                }

                //seek
                if(lseek(fd1, sizeof(dh) + cd.offset1 * sizeof(uint64_t),SEEK_SET)==-1){
                        err(7,"Error on seek");
                }

                if(read(fd1,&el1,sizeof(el1)) == -1){
                        err(4,"Error on read");
                }

                if(lseek(fd1,sizeof(dh)+cd.offset2*sizeof(uint64_t),SEEK_SET) ==-1){
                        err(7,"Error on seek");
                }

                if(read(fd1,&el2,sizeof(el2))==-1){
                        err(4,"Error on read");
				}

                        //compare
                if(cd.type==0){
                        if(el1>el2){
                                if(lseek(fd1,sizeof(dh)+cd.offset1*sizeof(uint64_t),SEEK_SET)== -1){
                                        err(7,"Error on read");
                                }

                                if(write(fd1,&el2,sizeof(el2)) != sizeof(el2)){
                                        err(8,"Error on write");
                                }

                                if(lseek(fd1,sizeof(dh)+cd.offset2*sizeof(uint64_t),SEEK_SET)==-1){
                                        err(7,"Error on read");
                                }

                                if(write(fd1,&el1,sizeof(el1)) != sizeof(el1)){
                                        err(8,"Error on write");
                                }
                        }

                }else{

                        if(el1<el2){
                                if(lseek(fd1,sizeof(dh)+cd.offset1*sizeof(uint64_t),SEEK_SET) == -1){
                                        err(7,"Error on lseek");
                                }

                                if(write(fd1,&el1,sizeof(el1))!=sizeof(el1)){
                                                err(8,"Error on write");
                                }

                                if(lseek(fd1,sizeof(dh)+cd.offset1*sizeof(uint64_t),SEEK_SET) == -1){
                                        err(7,"Error on seek");
                                }
                                if(write(fd1,&el2,sizeof(el2))!=sizeof(el2)){
                                        err(8,"Error on write");
                                }
                        }

                }

        }

        if(bytes_read == -1){
                err(4,"Error on read");
        }


        close(fd1);
        close(fd2);

        exit(0);
		}