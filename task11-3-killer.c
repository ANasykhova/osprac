#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct Server {
    long mtype;
    struct {
    	float msg;
    } info;
} serverbuf;

struct Client {
    long mtype;
    struct {
        pid_t pid;
        float msg;
    } info;
} clientbuf;

int main() {
    char pathname[] = "task11-3-server.c";
    int msqid;
    key_t key;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    clientbuf.mtype = 2;
    clientbuf.info.pid = getpid();
    clientbuf.info.msg = 0;

    printf("This program will kill server!\n");
    
    if (msgsnd(msqid, (struct clientbuf *) &clientbuf, sizeof(clientbuf.info), 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Killed!\n");

    return 0;
}

