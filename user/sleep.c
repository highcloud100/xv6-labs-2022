#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // argc : 인자 개수 
    // argv[0] => path 고정
    if(argc != 2){
        fprintf(2, "usage: sleep time\n");
        exit(1);
    }
    int time = atoi(argv[1]);
    sleep(time);
    exit(0);
}