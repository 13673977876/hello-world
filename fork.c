#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    /*int pid1 = fork();
	char *arr[] = {"ls","-la","..", NULL};
    if (pid1 == -1) {
        perror("fork1()");
        exit(EXIT_FAILURE);
    }
	
    if (pid1 == 0) {
        //execl("/bin/ls","ls","-l",".",NULL);
		execv("/bin/ls", arr);
		perror("execl()");
    }*/
	
    int pid2 = fork();
   	 
    if (pid2 == -1) {
        perror("fork2()");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // 第二个子进程执行的代码
        printf("我会跳舞！\n");
        getchar();  // 等待用户输入
        printf("pid2:%d, ppid:%d pgid:%d pgrp:%d sid:%d\n", getpid(), getppid()\
				,getpgid(0), getpgrp(), getsid(0));
        exit(EXIT_SUCCESS);
    }

    // 父进程执行的代码
    printf("我会飞！\n");
    //getchar();  // 等待用户输入
    printf("fpid:%d, fppid:%d pgid:%d pgrp:%d sid:%d\n", getpid(), getppid()\
			,getpgid(getpid()), getpgrp(),getsid(0));
    
    // 等待所有子进程结束
    //wait(NULL);  // 等待第一个子进程
    //wait(NULL);  // 等待第二个子进程
	_exit(0);
    return 0;
}
