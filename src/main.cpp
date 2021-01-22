#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include "tcpsvr.h"

using namespace std;
#define TCPSVRPORT 8888
#define MAKESHORT(a,b) ((a&0xff)|((b&0xff)<<8))
#define MAKEINT(a,b,c,d) ((a&0xff)|((b&0xff)<<8)|((c&0xff)<<16)|((d&0xff)<<24))

static void signal_action(int sig, siginfo_t* info, void* p)
{
	static int iSig11En = 1;
	struct sigaction act;
	sigset_t* mask = &act.sa_mask;
	int n = 0;
	printf("sig = %d\n",sig);
	if(11 == sig)
	{
		//printf("sig = 11 ........\n");
		pthread_exit((void*)syscall(SYS_gettid));
	 	sigwait(mask, &n);
		if (iSig11En)
		{
			iSig11En = 0;
		}
		else
		{
			//printf("return signal_action........\n");
			iSig11En = 1;
			return;
		}
	}
	else if((SIGUSR1==sig) || (SIGUSR2==sig))
	{
		//LogInfo("program exit");
		exit(0);
	}

	//printf("signal_action,sig = %d........\n",sig);	
}

static void block_bad_singals()
{
  	sigset_t   signal_mask;
	sigemptyset (&signal_mask);
	sigaddset (&signal_mask, SIGPIPE);
	
	if (pthread_sigmask (SIG_BLOCK, &signal_mask, NULL))
	{
    	//printf("block sigpipe error\n");
	}
}

void signal_Init(void)
{
	 struct sigaction act;

	 sigset_t* mask = &act.sa_mask;
	 act.sa_flags=SA_SIGINFO;     /** 设置SA_SIGINFO 表示传递附加信息到触发函数 **/
	 act.sa_sigaction=signal_action;
	 
	 block_bad_singals();
	 
	 // 在进行信号处理的时候屏蔽所有信号
	 sigemptyset(mask);   /** 清空阻塞信号 **/

	 //添加阻塞信号
	 sigaddset(mask, SIGABRT);
	 sigaddset(mask, SIGHUP);
	 sigaddset(mask, SIGQUIT);
	 sigaddset(mask, SIGILL);
	 sigaddset(mask, SIGTRAP);
	 sigaddset(mask, SIGIOT);
	 sigaddset(mask, SIGBUS);
	 sigaddset(mask, SIGFPE);
	 sigaddset(mask, SIGSEGV);
	 sigaddset(mask, SIGUSR1);
	 sigaddset(mask, SIGUSR2);

	 
	//安装信号处理函数
	 sigaction(SIGABRT,&act,NULL);
	 //sigaction(SIGEMT,&act,NULL);
	 sigaction(SIGHUP,&act,NULL);
	 sigaction(SIGQUIT,&act,NULL);
	 sigaction(SIGILL,&act,NULL);
	 sigaction(SIGTRAP,&act,NULL);
	 sigaction(SIGIOT,&act,NULL);
	 sigaction(SIGBUS,&act,NULL);
	 sigaction(SIGFPE,&act,NULL);
	 sigaction(SIGSEGV,&act,NULL);
	 sigaction(SIGUSR1,&act,NULL);
	 sigaction(SIGUSR2,&act,NULL);
	 /*
	  * linux重启或使用kill命令会向所有进程发送SIGTERM信号，所以不需要安装此信号的处理函数
	  */
	 sigaction(SIGINT,&act,NULL);

}
static int dealdata(const char* data,int datalen,int fd)
{
	printf("datalen = %d,fd = %d\n",datalen,fd);
	for(int i=0;i<datalen;i++)
	{
		 printf("0x%02x ",*(data+i));
	}
	printf("\n");
	int sendlen = write(fd,data,datalen);
	if(sendlen < 0)
	{
		printf("send error.\n");
	}
	return 0;
}
static int dealnotify(const char* data, int datalen)
{
	//固定40字节
	if(datalen != 40)
	{
		return -1;
	}
	int cmd,fd;
	char client[32];
	cmd = MAKEINT(*(data),*(data+1),*(data+2),*(data+3));
	fd = MAKEINT(*(data+4),*(data+5),*(data+6),*(data+7));
	memset(client,0,sizeof(client));
	memcpy(client,data+8,sizeof(client));
	printf("eventnotify:cmd = %d,fd = %d,client = %s\n",cmd,fd,client);
	return 0;

}
static void *worker(void *arg)
{
	StartTCPService(TCPSVRPORT,dealdata,dealnotify);
	return NULL;
}
int main(int argc,char *argv[])
{	
	//信号初始化
	signal_Init();
	pthread_t tid;
	if(pthread_create(&tid, NULL, worker, NULL) != 0)
	{
		printf("thread worker creat error.\n");
		return -1;
	}
	pthread_detach(tid);
	while(1)
	{
		sleep(5);
	}	
	return 0;
}
