#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

/* Описание алгоритма:
 * Два процесса, один pipe, один IPC семафор.
 * Начальное значение семафора = 2.
 * Процессы последовательно выполняют два действия N раз.
 * 1) Считывание сообщения из канала (кроме первой итерации parent цикла).
 * 2) Запись сообщения в канал.
 *
 * Синхронизация:
 * 1) Parent выполняет D(sem, 1) при входе и при выходе.
 * Проверяет есть ли письмо child и если есть, то читает его.
 * (ожидает запуска sem > 0, sem = 1 parent работает, sem = 0 parent завершает работу) 
 * 2) Child выполняет Z(sem) при  входе и A(sem, 2) при выходе.
 * Читает письмо parent и пишет ответ.
 * (ожидает запуска sem = 0, sem = 0 при работе child элемента, sem = 2 при выходе) 
 *
 * Так parent и child последовательно обмениваются данными по каналу в течение N раз.
 */

void A(int sem_id, int value) {
    if (value > 0) {
        struct sembuf buf;
        buf.sem_num = 0;
        buf.sem_flg = 0;
        buf.sem_op = value;

        if (semop(sem_id, &buf, 1) < 0) {
            printf("Can\'t wait for condition\n");
            exit(-1);
        }
    }
}

void D(int sem_id, int value) {
    if (value > 0) {
        struct sembuf buf;
        buf.sem_num = 0;
        buf.sem_flg = 0;
        buf.sem_op = -value;

        if (semop(sem_id, &buf, 1) < 0) {
            printf("Can\'t wait for condition\n");
            exit(-1);
        }
    }
}

void Z(int sem_id) {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_flg = 0;
    buf.sem_op = 0;

    if (semop(sem_id, &buf, 1) < 0) {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }
}

int main() {
    int fd[2], result;
    size_t size;
    key_t key;
    char  resstring[14];
    char pathname[] = "task9-3.c";
    int semid;

    int n;
    printf("Enter count of message:\n");
    scanf("%d", &n);
    if (n < 2) {
        printf("Count of message must be more or equals 2\n");
        exit(-1);
    }

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t create key\n");
        exit(-1);
    }

    if ((semid = semget(key, 1, 0666)) < 0) {
        printf("Can\'t find semaphore\n");
        if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
            printf("Can\'t get semid\n");
            exit(-1);
        }
        printf("Semophore successfully created\n");
    }

    A(semid, 2);

    if (pipe(fd) < 0) {
        printf("Can\'t open pipe\n");
        exit(-1);
    }

    result = fork();

    if (result < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (result > 0) {
        /* Parent process */
        for (size_t i = 0; i < n; i++) {
        D(semid, 1);

        if (i != 0) {
            size = read(fd[0], resstring, 14);
            if (size != 14) {
                printf("Parent:\tCan\'t write all string to pipe\n");
                exit(-1);
            }
            printf("Parent:\tI read message from child: %s\n", resstring);
        }

        char* msg = "Hello, child!";
        size = write(fd[1], msg, 14);
        if (size != 14) {
            printf("Parent:\tCan\'t write all string to pipe\n");
            exit(-1);
        }

        printf("Parent:\tI writing message...\n");
        for(long long i = 0; i < 100000000L; ++i);
            printf("Parent:\tI send message #%d to child: %s\n", i + 1, msg);
            printf("Parent:\tWait answer from child\n");
      
            D(semid, 1);
        }
        close(fd[0]);
        printf("Parent exit\n");
    } else {
        /* Child process */
        int counter = 0;
        for (int i = 0; i < n; i++) {
            Z(semid);

            size = read(fd[0], resstring, 14);
            if (size < 0) {
                printf("Child:\tCan\'t write all string to pipe\n");
                exit(-1);
            }

            printf("Child:\tI get message #%d from parent: %s\n", ++counter, resstring);

            char* msg = "Hello, parent";      
            size = write(fd[1], msg, 14);
            if (size != 14) {
                printf("child:\tI can\'t write all string");
                exit(-1);
            }

            if (i < n-1) {
                printf("Child:\tI writing message...\n");
                for(long long i = 0; i < 100000000L; ++i);
                printf("Child:\tI send message #%d to parent: %s\n", counter, msg);
                printf("Child:\tWait answer from parent\n");
            }

            A(semid, 2);
        }
        close(fd[1]);
        printf("Child exit\n");
    }

    return 0;
}

