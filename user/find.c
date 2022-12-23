#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char * path, char* target){
    char buf[512], *tempDir ;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        return;
    }
    if(fstat(fd, &st) < 0){
        close(fd);
        return ;
    }

    for(tempDir=path+strlen(path); tempDir>= path
         && *tempDir != '/'; tempDir--);
    
    //printf("tempDir %s\n", tempDir); // logging
    tempDir++;


    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        
        if(strcmp(tempDir, target) == 0){
            printf("%s\n", path);
        }
        /* code */
        break;
    case T_DIR:
        strcpy(buf, path);
        char*p = buf+strlen(buf);
        *p++ = '/'; // 경로 누적
        //printf("path %s\n", path);
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0) continue;
            memmove(p, de.name, DIRSIZ); // 경로 누적
            p[DIRSIZ] = 0; // 문자 끝
            //printf("PATH : %s\n", buf);
            //printf("de.name: %s\n", de.name);
            if(strcmp(".", de.name)==0 || strcmp("..", de.name)==0) continue;
            find(buf, target);
        }
        break;
    default:
        break;
    }
    close(fd);
}

int main(int argc, char*argv[]){
    if(argc == 2){
        find(".", argv[1]);
        exit(0);
    }
    else if(argc == 3){
        find(argv[1], argv[2]);
    }
    else{
        fprintf(2, "usage: find path target\n");
        exit(1);
    }
    return 0;
}