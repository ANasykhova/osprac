#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a = 0;

void *mythread(void *dummy) {
    pthread_t mythid;
    mythid = pthread_self();

    a = a + 1;
    printf("Thread %u, Calculation result = %d\n", mythid, a);
    return NULL;
}

int main() {
    pthread_t thid, mythid, thid2;
    int result;

    result = pthread_create (&thid, (pthread_attr_t *)NULL, mythread, NULL);
    if (result != 0) {
        printf ("Error on thread create, return value = %d\n", result);
        exit(-1);
    }
    printf("Second thread created, thid = %u\n", thid);

    result = pthread_create (&thid2, (pthread_attr_t *)NULL, mythread, NULL);
    if (result != 0) {
        printf ("Error on thread create, return value = %d\n", result);
        exit(-1);
    }
    printf("Third thread created, thid = %u\n", thid2);

    mythid = pthread_self();

    a = a + 1;

    printf("Thread %u, Calculation result = %d\n", mythid, a);

    pthread_join(thid, (void **)NULL);
    pthread_join(thid2, (void **)NULL);

    return 0;
}

