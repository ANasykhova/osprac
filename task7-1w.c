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

    int file_size = 0;
    int *file_size_ptr;

    FILE *file;
    file = fopen(pathname, "r");
    fseek(file, 0L, SEEK_END);

    file_size = ftell(file);
    rewind(file);

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, sizeof(int) + file_size * sizeof(char), 0666|IPC_CREAT)) < 0) {
        printf("Can\'t create shared memory\n");
        exit(-1);
    }

    if ((file_size_ptr = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)) {
        printf("Can\'t attach shared memory\n");
        exit(-1);
    }

    *file_size_ptr = file_size;
    source_code = (char*)(file_size_ptr + 1);

    for (int i = 0; i < file_size; ++i) {
        source_code[i] = fgetc(file);
    }

    fclose(file);

    if (shmdt(file_size_ptr) < 0) {
        printf("Can\'t detach shared memory\n");
        exit(-1);
    }

    return 0;
}

