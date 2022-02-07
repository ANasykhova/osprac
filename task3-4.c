#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[], char *envp[]) {
    pid_t pid, ppid;
    pid = fork();

    if (pid == -1) {
        printf("Failed to create process");
        exit(-1);
    } else if (pid == 0) {
        printf("Child process, pid: %d, ppid: %d\n", (int)getpid(), (int)getppid());
        (void) execle("/bin/cat", "/bin/cat", "task3-3.c", 0, envp);
        printf("Error on program start.\n");
    } else {
        printf("Parent process, pid: %d, ppid: %d\n", (int)getpid(), (int)getppid());
    }

    return 0;
}

