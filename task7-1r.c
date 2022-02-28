#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "stdlib.h"
#include <sys/shm.h>
#include <sys/ipc.h>

int main() {
    int shmid;
    char pathname[] = "task7-1w.c";
    key_t key;
    char *source_code;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    int file_size = 0;

    if ((shmid = shmget(key, sizeof(int) + file_size * sizeof(char), 0666|IPC_CREAT)) < 0) {
        printf("Can\'t create shared memory\n");
        exit(-1);
    }

    int *file_size_ptr;

    if ((file_size_ptr = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
        printf("Can\'t attach shared memory\n");
        exit(-1);
    }

    file_size = *file_size_ptr;
    source_code = (char*)(file_size_ptr + 1);

    for (int i = 0; i < file_size; ++i){
        putchar(source_code[i]);
    }

    if (shmdt(file_size_ptr) < 0) {
        printf("Can\'t detach shared memory\n");
        exit(-1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("Can\'t delete shared memory\n");
        exit(-1);
    }

    return 0;
}

