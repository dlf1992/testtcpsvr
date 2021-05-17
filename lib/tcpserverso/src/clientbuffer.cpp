/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: clientbuffer.cpp
* @brief: TCP连接客户端数据缓冲区处理
* @author: dinglf
* @date: 2021-05-17
* @history:
*******************************************************************/
#include "clientbuffer.h"

CientBuffer::CientBuffer(unsigned short port):tcp_port(port)
{
	pTRingBuffer[MAX_RingBuffer] = {NULL};
	pClientlocker[MAX_RingBuffer] = {NULL};
	map_TRingBuffer.clear();
	map_Clientlocker.clear();
	task_callback = NULL;
	read_packet = NULL;
}
CientBuffer::~CientBuffer()
{
	task_callback = NULL;
	read_packet = NULL;
	destroyall();
}
TRingBuffer* CientBuffer::findtask(int fd)
{
	TRingBuffer *pbuffer = NULL;
	TRingBuffermaplocker.mutex_lock();
	map<int,TRingBuffer*>::iterator iter;
	iter = map_TRingBuffer.find(fd);
	if(iter != map_TRingBuffer.end())
	{
		////printf("found fd,fd = %d\n",fd);
		pbuffer = iter->second;
	}
	else
	{
		//printf("not found fd,fd = %d\n",fd);
		for(int i=0;i<MAX_RingBuffer;i++)
		{
			if(pTRingBuffer[i] == NULL)
			{
				//printf("fd = %d,pTRingBuffer[%d]\n",fd,i);
				pTRingBuffer[i] = new TRingBuffer;
				if(pTRingBuffer[i] != NULL)
				{
					pTRingBuffer[i]->Create(MAX_BUFFER);
					map_TRingBuffer.insert(make_pair(fd,pTRingBuffer[i]));
					pbuffer = pTRingBuffer[i];
				}
				break;
			}
		}
	}
    TRingBuffermaplocker.mutex_unlock();
	return pbuffer;
}
mutex_locker* CientBuffer::findclientlocker(TRingBuffer *pTRingBuffer)
{
	mutex_locker *pClientlock = NULL;
	TRingBuffermaplocker.mutex_lock();
	map<TRingBuffer*,mutex_locker*>::iterator iter;
	iter = map_Clientlocker.find(pTRingBuffer);
	if(iter != map_Clientlocker.end())
	{
		////printf("found pTRingBuffer\n");
		pClientlock = iter->second;
	}
	else
	{
		//printf("not found pTRingBuffer\n");
		for(int i=0;i<MAX_RingBuffer;i++)
		{
			if(pClientlocker[i] == NULL)
			{
				//printf("pClientlocker[%d]\n",i);
				pClientlocker[i] = new mutex_locker;
				if(pClientlocker[i] != NULL)
				{
					map_Clientlocker.insert(make_pair(pTRingBuffer,pClientlocker[i]));
					pClientlock = pClientlocker[i];
				}
				break;
			}
		}
	}
    TRingBuffermaplocker.mutex_unlock();
	return pClientlock;
}
void CientBuffer::clearRingBuffer(int fd)
{
	//printf("clearRingBuffer,fd = %d\n",fd);
	//清除锁，清除缓冲区
	TRingBuffermaplocker.mutex_lock();
	map<int,TRingBuffer*>::iterator iter;
	iter = map_TRingBuffer.find(fd);
	if(iter != map_TRingBuffer.end())
	{
		//找到fd
		//printf("found fd\n");
		map<TRingBuffer*,mutex_locker*>::iterator iter1;
		iter1 = map_Clientlocker.find(iter->second);
		if(iter1 != map_Clientlocker.end())
		{
			//找到pTRingBuffer，清除锁
			//printf("found pTRingBuffer\n");
			for(int i=0;i<MAX_RingBuffer;i++)
			{
				if(iter1->second == pClientlocker[i])
				{
					//printf("fd = %d,pClientlocker[%d]\n",fd,i);
					if(pClientlocker[i] != NULL)
					{
						delete pClientlocker[i];
						pClientlocker[i] = NULL;
					}
					break;
				}	
			}
			map_Clientlocker.erase(iter1);
		}
		//清除缓冲区
		for(int i=0;i<MAX_RingBuffer;i++)
		{
			if(iter->second == pTRingBuffer[i])
			{
				//printf("fd = %d,pTRingBuffer[%d]\n",fd,i);
				if(pTRingBuffer[i] != NULL)
				{
					delete pTRingBuffer[i];
					pTRingBuffer[i] = NULL;
				}
				break;
			}
		}
		map_TRingBuffer.erase(iter);
	}
	TRingBuffermaplocker.mutex_unlock();	
}
void CientBuffer::destroyall()
{
	//delete两个指针数组，置NULL
	TRingBuffermaplocker.mutex_lock();
	for(int i=0;i<MAX_RingBuffer;i++)
	{
		//printf("fd = %d,pTRingBuffer[%d]\n",fd,i);
		if(pTRingBuffer[i] != NULL)
		{
			delete pTRingBuffer[i];
			pTRingBuffer[i] = NULL;
		}
	}
	for(int i=0;i<MAX_RingBuffer;i++)
	{
		//printf("fd = %d,pTRingBuffer[%d]\n",fd,i);
		if(pClientlocker[i] != NULL)
		{
			delete pClientlocker[i];
			pClientlocker[i] = NULL;
		}
	}
	map_TRingBuffer.clear();
	map_Clientlocker.clear();
	TRingBuffermaplocker.mutex_unlock();
}

