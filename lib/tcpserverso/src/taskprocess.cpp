/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: taskprocess.cpp
* @brief: TCP通信服务端数据任务处理实现
* @author: dinglf
* @date: 2020-12-21
* @history:
*******************************************************************/
#include "taskprocess.h"

//静态成员变量初始化
TRingBuffer* Task::pTRingBuffer[MAX_RingBuffer] = {NULL};
mutex_locker* Task::pClientlocker[MAX_RingBuffer] = {NULL};
ringbuffermap Task::map_TRingBuffer;
lockermap Task::map_Clientlocker;
mutex_locker Task::TRingBuffermaplocker;
ptcpFun Task::task_callback = NULL;

void Task::processtask(char *buffer,int buflen,int fd)
{
	//printf("enter processtask,buflen = %d,fd = %d.\n",buflen,fd);
	//for(int i=0;i<buflen;i++)
	//{
	//	 printf("0x%02x ",*(buffer+i));
	//}
	//printf("\n");
	char szPacket[2048];
	int iPacketlen = 0;

	TRingBuffer *pbuffer = NULL;
	pbuffer = findtask(fd);
	if(NULL == pbuffer)
	{
		//printf("pbuffer=NULL.\n");
		return;
	}	
	mutex_locker *pclientlocker = NULL;
	pclientlocker = findclientlocker(pbuffer);
	if(NULL == pclientlocker)
	{
		//printf("pclientlocker=NULL.\n");
		return;		
	}
	pclientlocker->mutex_lock();
	if(buflen>0)
	{
		if(!pbuffer->WriteBinary((uint8 *)buffer, buflen))
		{
			//printf("can not write ringbuffer,buflen = %d.\n",buflen);
		}
	}
	while(pbuffer->GetMaxReadSize()>0)
	{
		iPacketlen = ReadPacket(pbuffer,szPacket, sizeof(szPacket));
		if(iPacketlen> 0)
		{
			//printf("Read packet success,iPacketlen = %d\n",iPacketlen);
			//具体协议处理
			if(task_callback != NULL)
			{
				task_callback(szPacket,iPacketlen,fd);
			}
			iPacketlen = 0;
		}
		else
		{
			//printf("Read packet failed\n");
			//一直读取包，直到读取不成功，退出；也可能一次都不成功
			break;
			//m_readBuffer.Clear();
		}
	}
	pclientlocker->mutex_unlock();	
}
int Task::ReadPacket(TRingBuffer *clientbuffer,char* szPacket, int iPackLen)
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
		if(packetlen > MAX_BUFFER)
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
TRingBuffer* Task::findtask(int fd)
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
mutex_locker* Task::findclientlocker(TRingBuffer *pTRingBuffer)
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
void Task::clearRingBuffer(int fd)
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
			map_Clientlocker.erase(iter1);
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
		}
		//清除缓冲区
		map_TRingBuffer.erase(iter);
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
	}
	TRingBuffermaplocker.mutex_unlock();	
}