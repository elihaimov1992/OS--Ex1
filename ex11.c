/**
 *  Exp : Getting two files and checking if they equals
 * @param argc
 * @param argv
 * @return 2 - if files are equals, else 1
 *
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char *argv[]) {


    if (argc != 3) {
        perror("Error : have not 2 files");
        return 1;
    }
    else{

        char buffer1, buffer2;
        int fd_s_1,fd_s_2;

        // only read a two files
        int fd_1= open(argv[1],O_RDONLY);
        int fd_2= open(argv[2],O_RDONLY);


        // check exists of files
        if(fd_1<0 || fd_2<0){
            perror("Error : one of the files aren't exist\n");
            return 1;
        }


        // compare per chars between fd_1 & fd_2
        while((fd_s_1= read(fd_1 , &buffer1 , 1))==1 && (fd_s_2 = read(fd_2 , &buffer2 , 1))==1 ){
            if(buffer1!=buffer2){
                perror("Error : The files aren't equals\n");
                return 1;
            }
        }

        // compare length of fd_1 & fd_2 - if are equals
        if((fd_s_1= read(fd_1 , &buffer1 , 1))==1 || (fd_s_2 = read(fd_2 , &buffer2 , 1))==1){
            if(fd_s_1 != fd_s_2){
                perror("Error : The length of files aren't equals\n");
                return 1;
            }
        }


        // checking if the files were closed
        if(close(fd_1)<0 || close(fd_2)<0){
            perror("Error : The files aren't closed\n");
            return 1;
        }

        printf("2\n");
        return 2;
    }

}

