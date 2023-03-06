#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    pid_t pid = fork();

    if(pid == 0){
        fork();
        fork();
        fork();
    }

    printf("%d\n", getpid());
    return 0;
}