#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char*argv[]){
    if(argc != 1){
         fprintf(2, "usage: pingpog\n");
        exit(1);
    }
    int p[2];

    pipe(p);

    if(fork()==0){ // child
        // p[1] write
        int buf[1];
        read(p[0],buf,1);
        
        printf("%d: received ping\n", buf[0]);

        buf[0] = getpid();
        write(p[1],buf, 1);
    }
    else{
        int buf[1];
        buf[0] = getpid();
        write(p[1],buf, 1);
        wait(0);

        read(p[0], buf, 1);
        printf("%d: received pong\n", buf[0]);
    }
    close(p[0]);
    close(p[1]);
    exit(0);
}