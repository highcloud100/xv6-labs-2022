#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char*argv[]){
    if(argc < 2){
        fprintf(2, "usage: xargs cmd\n");
        exit(1);
    }
    
    char*temp_argv[MAXARG+1];
    int i;

    temp_argv[0] = argv[1];
    
    for(i=1;i<argc-1;i++){
        temp_argv[i] = argv[i+1];
    }


    char* buf = malloc(MAXARG*30);
    char* temp = buf;
    int argSize = 0;
    int argIndex = 0;
    while(read(0, temp,1))
    {   
        //printf("buf : %s temp : %c\n", buf, *temp);
        if(*temp != ' ' && *temp != '\n'){ // 공백이나 \n이 아니면 temp포인터 이동
            temp++;
            argSize++; // 이동한 거리 누적
            continue;
        }
        else{ // 공백이나 \n 나오면
            char save=*temp; // 밑에서 \n인지 판단하기 위해 저장
            *temp = '\0'; // end of str
            
            temp_argv[i] = temp-argSize; // str의 시작 주소를 argv 리스트에 저장
            //printf("temp_argv[%d] = %s\n", i, temp-argSize);

            argSize=0; // argSize 초기화
            i++; // argv 리스트 인덱스 증가
            temp++; // \0 다음을 가르킴

            if(save=='\n'){ // \n이면 fork후 실행
                if(fork()==0){
                    // for(int k=0;k<i;k++){ // 모든 temp_argv 프린트
                    //     printf("%d : %s | ",k,  temp_argv[k]);
                    // }
                    //printf("%d\n", argIndex);

                    exec(temp_argv[0], temp_argv+argIndex); // 인자 값을 잘라서 전달
                    fprintf(2, "exec: error\n");
                    exit(1);
                }
                else{
                    wait(0);
                    temp = buf; // 버퍼 맨 앞으로 이동 / 초기화 
                    
                    temp_argv[i] = argv[1]; // path
                    argIndex=i++; // 잘라서 전달할 부분 지정

                    //temp_argv 에 기존 값 저장
                    for(int j=2;j<argc;j++){
                        temp_argv[i++] = argv[j];
                        //printf("--- %s ---\n", argv[j]);
                    }
                }
            }
            
        }
    }
    free(buf);
    return 0;
}