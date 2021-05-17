/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: taskprocess.h
* @brief: TCP通信服务端数据任务处理
* @author: dinglf
* @date: 2020-12-21
* @history:
*******************************************************************/
#ifndef _TASK_PROCESS_
#define _TASK_PROCESS_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include <sys/time.h>
#include "clientbuffer.h"



using namespace std;

class BaseTask
{
public:
	BaseTask(){}
	virtual ~BaseTask(){}
	virtual void doit() = 0;
};
 
class Task : public BaseTask
{
private:
	int sockfd;
	char order[MAX_BUFFER];
	int orderlen;
	CientBuffer *pclientbuf;
public:
	Task(CientBuffer *pclientbuffer,char *str,int readlen, int fd) : sockfd(fd)
	{
		pclientbuf = pclientbuffer;
		memset(order, 0, MAX_BUFFER);
		memcpy(order,str,readlen);
		orderlen = readlen;
	}
	void doit()  //任务的执行函数
	{
		//do something of the order
		processtask(pclientbuf,order,orderlen,sockfd);
	}
	void processtask(CientBuffer *pclientbuffer,char *buffer,int buflen,int fd);
	int ReadPacket(TRingBuffer *clientbuffer,char* szPacket, int iPackLen);
};
#endif