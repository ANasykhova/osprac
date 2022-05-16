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
        printf("Can't generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can't get msqid\n");
        exit(-1);
    }

    clientbuf.mtype = 1;
    clientbuf.info.pid = getpid();

    float msg;
    printf("Enter float number: \n");
    scanf("%f", &msg);

    clientbuf.info.msg = msg;
    
    int len = sizeof(clientbuf.info);
    printf("#%d client sended this: %f\n", clientbuf.info.pid, clientbuf.info.msg);
    if (msgsnd(msqid, (struct clientbuf *) &clientbuf, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Waiting for response\n");

    if (msgrcv(msqid, &serverbuf, sizeof(serverbuf.info), getpid(), 0) < 0) {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }

    printf("Server returned this: %f\n", serverbuf.info.msg);

    return 0;
}

