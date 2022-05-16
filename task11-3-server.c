#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <errno.h>

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

    int pid_file = open("task11-3-server.c", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);

    if (rc) {
	    if(EWOULDBLOCK == errno) {
		    printf("Another instance is running!\n");
		    exit(-1);
	    }
    }

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1) {
        int maxlen = sizeof(clientbuf.info);
        if (msgrcv(msqid, (struct clientbuf *) &clientbuf, maxlen, -2, 0) < 0) {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }

	    if (clientbuf.mtype == 2) {
		    printf("Program killed!\n");
		    exit(-1);
	    }

        printf("%d client sent this: %f \n", clientbuf.info.pid, clientbuf.info.msg);

        serverbuf.mtype = clientbuf.info.pid;
        serverbuf.info.msg = clientbuf.info.msg * clientbuf.info.msg;

        int len = sizeof(serverbuf.info);
        if (msgsnd(msqid, (struct server *) &serverbuf, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }

        printf("Sent response to client!\n");
    }
    return 0;
}

