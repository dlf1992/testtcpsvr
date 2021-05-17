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
#define TCPSVRPORT1 5555

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
	printf("dealdata,datalen = %d,fd = %d\n",datalen,fd);
	for(int i=0;i<datalen;i++)
	{
		 printf("0x%02x ",*(data+i));
	}
	printf("\n");
	int sendlen = write(fd,data,datalen);
	if(sendlen < 0)
	{
		printf("dealdata,send error.\n");
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
	printf("dealdata,eventnotify:cmd = %d,fd = %d,client = %s\n",cmd,fd,client);
	return 0;

}
static int dealdata1(const char* data,int datalen,int fd)
{
	printf("dealdata1,datalen = %d,fd = %d\n",datalen,fd);
	for(int i=0;i<datalen;i++)
	{
		 printf("0x%02x ",*(data+i));
	}
	printf("\n");
	int sendlen = write(fd,data,datalen);
	if(sendlen < 0)
	{
		printf("dealdata1,send error.\n");
	}
	return 0;
}
static int dealnotify1(const char* data, int datalen)
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
	printf("dealnotify1,eventnotify:cmd = %d,fd = %d,client = %s\n",cmd,fd,client);
	return 0;
}

static int ReadPacket(TRingBuffer *clientbuffer,char* szPacket, int iPackLen)
{
	int iRet = 0;

	int iStartPos = 0;
	int iStopPos = 0;
	int packetlen = 0;//packetlen 总长度
	unsigned char ch1;
	unsigned char ch2;
	unsigned char ch3;
	unsigned char packetlenlow;
	unsigned char packetlenhigh;
	//unsigned char packetlen_get;
	
	if(NULL == clientbuffer)
		return iRet;

	if (!clientbuffer->FindChar(0x23, iStartPos)) //find #
	{
        //0x23都查找不到 肯定是无效数据 清空
		//printf("can not find #,clear ringbuffer.\n");
        clientbuffer->Clear();	
		return iRet;		
	}
	//printf("iStartPos = %d\n",iStartPos);
	//printf("clientbuffer->GetMaxReadSize() = %d\n",clientbuffer->GetMaxReadSize());
	if(clientbuffer->GetMaxReadSize() <= iStartPos+6)
	{
		//printf("can not find total protocol.\n");
		//丢弃#前面的数据
		clientbuffer->ThrowSomeData(iStartPos);
		return iRet;
	}
	
	if((!clientbuffer->PeekChar(iStartPos+1,ch1))\
		||(!clientbuffer->PeekChar(iStartPos+2,ch2))\
		||(!clientbuffer->PeekChar(iStartPos+3,ch3)))
	{
		//printf("can not find char.\n");
		return iRet;
	}
	if((ch1==0x23)&&(ch2==0x23)&&(ch3==0x23))
	{
		//丢弃#前面的数据
		clientbuffer->ThrowSomeData(iStartPos);
		iStartPos = 0;
		//数据包长度
		clientbuffer->PeekChar(iStartPos+4,packetlenlow);
		clientbuffer->PeekChar(iStartPos+5,packetlenhigh);
		//clientbuffer->PeekChar(iStartPos+5,packetlen_get);
		packetlen = packetlenlow|(packetlenhigh<<8);
		//printf("packetlen = %d\n",packetlen);
		if(packetlen > 2048)
		{
			//处理异常数据
			clientbuffer->Clear();	
			return iRet;
		}
		iStopPos = iStartPos+packetlen;
		if (iStopPos <= clientbuffer->GetMaxReadSize())
		{
			if (iStopPos > iPackLen) clientbuffer->ThrowSomeData(iStopPos); //数据超长，丢弃
			else if (clientbuffer->ReadBinary((uint8*)szPacket, iStopPos))
			{
				iRet = packetlen;
				//m_readBuffer.Clear();
			}
		}
	}
	else
	{
		//长度够，但是不完全符合格式，清空
		//printf("imcomplete with data format,clear all.\n");
		clientbuffer->Clear();	
		return iRet;
	}

	return iRet;	
}
static void *worker(void *arg)
{
	StartTCPService(TCPSVRPORT,dealdata,dealnotify,ReadPacket);
	return NULL;
}
static void *worker1(void *arg)
{
	StartTCPService(TCPSVRPORT1,dealdata1,dealnotify1,ReadPacket);
	return NULL;
}

int main(int argc,char *argv[])
{	
	//信号初始化
	signal_Init();
	pthread_t tid,tid1;
	if(pthread_create(&tid, NULL, worker, NULL) != 0)
	{
		printf("thread worker creat error.\n");
		return -1;
	}
	pthread_detach(tid);
	if(pthread_create(&tid1, NULL, worker1, NULL) != 0)
	{
		printf("thread worker1 creat error.\n");
		return -1;
	}
	pthread_detach(tid1);	
	while(1)
	{
		sleep(5);
	}	
	return 0;
}
