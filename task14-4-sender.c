#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

int success = 0;

void my_handler(int nsig) {
    success = 1;
}

int main(void) {
    int pid;
    int num;
    int bits[32];
    int sign;
  
    (void) signal(SIGUSR1, my_handler);

    printf("My pid: %d\n", getpid());
    printf("Enter pid: ");
    scanf("%d", &pid);
    printf("Enter number: ");
    scanf("%d", &num);

    while (!success);

    sign = num < 0;
    if (sign) {
        num = num * (-1);
        num--;
    }
    bits[0] = sign;

    for (int i = 0; i < 31; ++i) {
        bits[31 - i] = (num % 2) ^ sign;
        num /= 2;
    }

    for (int i = 0; i < 32; ++i) {
        if (bits[i])
            kill(pid, SIGUSR2);
        else
            kill(pid, SIGUSR1);
        success = 0;

        while(!success);
    }

    printf("Sent the number\n");
  
    return 0;
}

