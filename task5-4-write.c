#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    (void)umask(0);
    int fd;
    size_t size;
    char fileName[] = "example.fifo";
 
    printf("Creating FIFO\n");

    if ((fd = open(fileName, O_WRONLY)) < 0){
        printf("Can\'t open FIFO for writing!\n");
        exit(-1);
    }

    size = write(fd, "Hello, world!", 14);

    if (size != 14){
        printf("Can\'t write all strings!\n");
        exit(-1);
    }

    if (close(fd) < 0) {
        printf("Can\'t close FIFO\n");
        exit(-1);
    }

    return 0;
}

