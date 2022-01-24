#include "stdio.h"
#include "unistd.h"

int main() {
    int user_id = getuid();
    int group_id = getgid();
    printf("User ID: %d, Group ID: %d\n", user_id, group_id);
}

