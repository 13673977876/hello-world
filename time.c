#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <sys/file.h>

#define LOCK_FILE "/var/run/mydaemon.pid"
#define BUFFSIZE 128
static int my_daemon(void);
static int runing_already(void);
int main(int argc, char *argv[]){
	int pid;
	time_t t;
	struct tm *tm_t = NULL;
	char buff[BUFFSIZE];
	FILE *file = NULL;
	//创建守护进程
	if(my_daemon() == -1){
		fprintf(stderr, "守护进程创建失败！");
		return 1;
	}

	//单例进程
	if(-1 == runing_already()){
		syslog(LOG_ERR, "进程正在运行");
		return 1;
	}

	//守护进程执行语句（所有的输入到终端的数据，都会输入到/dev/null中）
	file = fopen(argv[1], "w");
	if(file == NULL){
		syslog(LOG_ERR, "fopen():file%s", strerror(errno));
		closelog();
		return -1;
	}
	while(1){
		fseek(file, 0, SEEK_SET); // rewind(file);
		t = time(NULL);	
		tm_t = localtime(&t);
		strftime(buff, BUFFSIZE, "%y/%m/%d %H:%M:%S\n", tm_t);
		fputs(buff, file);  //全缓存
		fflush(NULL);
		sleep(1);
	}
	fclose(file);

	return 0;
}

static int my_daemon(void)
{
	int pid;
	int fd;
	openlog(NULL, LOG_PID, LOG_DAEMON);
	pid = fork();
	if(pid < 0){
		perror("fork()");
		return -1;
	}
	else if(pid > 0){
		_exit(0);
	}
	if(-1 == setsid()){
		perror("getsid()");
		return -1;
	}
	chdir("/");
	umask(0);

	fd = open("/dev/null", O_RDWR);
	if( -1 == fd){
		syslog(LOG_ERR, "open():%s", strerror(errno));
		closelog();
		return -1;
	}
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	if(fd > 2){
		close(fd);
	}
	return 0;
}

//单例守护进程

static int runing_already(void)
{
	int fd;
	char buff[BUFFSIZE] = {};
	fd = open(LOCK_FILE, O_WRONLY | O_CREAT, 0666);
	if(fd == -1){
		syslog(LOG_ERR, "open():%s", strerror(errno));
		return -1;
	}
	if(flock(fd, LOCK_EX | LOCK_NB) == -1)
	{
		syslog(LOG_ERR, "open():%s", strerror(errno));
		close(fd);
		return -1;
	}

	//调用进程是唯一的
	snprintf(buff, BUFFSIZE, "%d", getpid());
	ftruncate(fd, 0);
	write(fd, buff, strlen(buff));

	return 0;
}
