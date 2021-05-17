/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: clientbuffer.h
* @brief: tcp连接客户端数据缓冲区
* @author: dinglf
* @date: 2021-05-17
* @history:
*******************************************************************/
#ifndef _CLIENT_BUFFER_H_
#define _CLIENT_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include <sys/time.h>

#include "locker.h"
#include "RingBuffer.h"

using namespace std;

typedef  int (*ptcpFun)(const char *,int,int);
typedef  int (*pReadPacketFun)(TRingBuffer*,char*,int);


#define MAX_RingBuffer 100
#define MAX_BUFFER 2048  //Buffer的最大字节

//typedef map<int,int> clienttypemap;//fd type 
typedef map<int,TRingBuffer*> ringbuffermap;
typedef map<TRingBuffer*,mutex_locker*> lockermap;

class CientBuffer
{
private:
	unsigned short tcp_port;
	TRingBuffer *pTRingBuffer[MAX_RingBuffer]; //环形队列指针数组
	mutex_locker *pClientlocker[MAX_RingBuffer]; //锁每个客户端的环形队列
	ringbuffermap map_TRingBuffer;
	lockermap map_Clientlocker;
	mutex_locker TRingBuffermaplocker; //互斥锁,锁缓冲区	
public:
	ptcpFun task_callback;
	pReadPacketFun read_packet;
public:
	CientBuffer(unsigned short port);
	~CientBuffer();
	TRingBuffer* findtask(int fd);//通过fd查找环形队列指针，如果没有就new
	mutex_locker* findclientlocker(TRingBuffer *pTRingBuffer);
	void clearRingBuffer(int fd);
	void destroyall();//析构时调用，释放所有缓冲区，清空所有map
};


#endif

