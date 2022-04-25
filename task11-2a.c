/*
 * Programs are sequentially sending to each other messages.
 * Integer value is initially 1, after each received message programs multiply this value by
 * certain multiplier (2 for a, 3 for b) and send result value back to the other program.
 * 
 * - Program a sends messages with type 1, b - 2.
 * - Program a sends 5 informative messages and one last message to stop messaging. 
 *   Program b listens the queue in an infinite loop. Exit condition is <message.info.mnumber = 0>.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct mymsgbuf {
    long mtype;
    struct {
        char  mtext[30];
        int   mnumber;
    } info;
} mybuf;

void send_message(int msqid, int number) {
    mybuf.mtype = 1;
    strcpy(mybuf.info.mtext, "Hello from program a!");
    mybuf.info.mnumber = number;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.info), 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }
}

void send_last_message(int msqid) {
    mybuf.mtype = 1;
    strcpy(mybuf.info.mtext, "Last message from program a!");
    mybuf.info.mnumber = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.info), 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }
}

void receive_message(int msqid) {
    if (msgrcv(msqid, (struct msgbuf *) &mybuf, sizeof(mybuf.info), 2, 0) < 0) {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }
}

int main(void) {
    int msqid;
    char pathname[] = "task11-2a.c";
    key_t  key;
    int i;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    int multiplier_a = 2;
    int current_value = 1;

    /* Send and receive information */

    for (i = 0; i < 5; i++) {
        send_message(msqid, current_value);

        receive_message(msqid);

        printf("Program a: type = %ld, text = %s, value = %d\n", mybuf.mtype, mybuf.info.mtext, mybuf.info.mnumber);
    
        current_value = mybuf.info.mnumber * multiplier_a;
    }

    /* Send the last message */

    send_last_message(msqid);

    return 0;
}

