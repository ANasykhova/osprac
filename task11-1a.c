#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

int main(void) {
    int msqid;      // IPC descriptor for the message queue
    char pathname[] = "task11-1a.c"; // The file name used to generate the key.
                // A file with this name must exist in the current directory.
    key_t key;  // IPC key
    int i, len; // Cycle counter and the length of the informative part of the message

    struct mymsgbuf { // Custom structure for the message
        long mtype;
        struct {
            char  mtext[81];
            int   mnumber;
        } info;
    } mybuf;

    if ((key = ftok(pathname,0)) < 0) {
        printf("Can\'t generate key\n");
        exit(-1);
    }
    //
    // Trying to get access by key to the message queue, if it exists,
    // or create it, with read & write access for all users.
    //
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    const char *msgs[] = {"This", "is", "text", "message", "from", "task11-1a.c"};

    /* Send information */

    for (i = 0; i < 6; i++) {
    //
    // Fill in the structure for the message and
    // determine the length of the informative part.
    //
        mybuf.mtype = 1;
        strcpy(mybuf.info.mtext, msgs[i]);
        mybuf.info.mnumber = i;
        len = sizeof(mybuf.info);
    //
    // Send the message. If there is an error,
    // report it and delete the message queue from the system.
    //
        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
            printf("Can\'t send message to queue\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
    }

    /* Send the last message */

    mybuf.mtype = LAST_MESSAGE;
    len = 0;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0) {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    return 0;
}
