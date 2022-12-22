#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char*argv[]){

    int p[2];
    pipe(p);
    int buf[34];
    for(int i=2;i<36;i++){
        buf[i-2] = i;
    }

    write(p[1], buf, 34);

    while(1){
        if(fork()==0){

            int readSize = read(p[0], buf, 34);
            
            //printf("readSize: %d\n", readSize);
            
            // for(int i=0;i<readSize;i++){
            //     printf("%d ", buf[i]);
            // }printf("\n");

            printf("prime %d\n", buf[0]);
            
            int prime = buf[0];
            int size=0;
            for(int i=0;i<readSize;i++){
                if(buf[i] % prime){
                    buf[size] = buf[i];
                    size++;
                }
            }
            close(p[0]);
            close(p[1]);
            pipe(p);
            write(p[1], buf, size);

            if(size==0){
                exit(0);
                break;   
            }

        }
        else{
            wait(0);
            break;
        }
    }

    return 0;
}