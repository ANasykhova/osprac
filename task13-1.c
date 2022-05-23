#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/mman.h>

char* getFileName(int i, int withTemp) {
    char* filename = malloc(sizeof("100"));

    if (withTemp == 0) {
        sprintf(filename, "temp/%03d", i);
    }
    else {
        sprintf(filename, "%03d", i);
    }

    return filename;
}

int createFile(char* name) {
    FILE* fd = fopen(name, "w");

    if (fd == NULL) {
        printf("File opening failed!\n");
        return 1;
    }

    fclose(fd);

    return 0;
}

int main(int argc, char* argv[]) {
    char* dir_name = "temp";
    printf("Program will look at the directory: %s\n", dir_name);

    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
       printf("Create directory with name \"temp\"\n");
    } else {
       printf("Directory \"temp\" was deleted and created again\n");
       system("rm -r temp");
    }
    system("mkdir temp");

    char* firstName = getFileName(0, 0);
    if (createFile(firstName)) {
        free(firstName);
        printf("File opening failed!\n");
        exit(1);
    }

    free(firstName);

    int counter = 1;

    while(1) {
        char* prevFile = getFileName(counter - 1, 1);
        char* nextFile = getFileName(counter, 0);

        if (symlink(prevFile, nextFile)) {
            free(prevFile);
            free(nextFile);
            printf("Error with symlink\n");
            exit(-1);
        }

        FILE* newFile = fopen(nextFile, "r");
        if (newFile == NULL) {
            printf("Max recursion depth: %d\n", --counter);
            free(prevFile);
            free(nextFile);
            exit(1);
         }

        fclose(newFile);
        free(prevFile);
        free(nextFile);

        counter++;
    }

    if (closedir(dir) != 0) {
        printf("Can\'t close directory!\n");
        return(-1);
    }

    return 0;
}

