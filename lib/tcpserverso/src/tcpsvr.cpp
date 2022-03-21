#include <stdio.h>
#include <stdlib.h>
#include "tcpserver.h"
#include "tcpsvr.h"
#include "taskprocess.h"

map<unsigned short,TcpServer*> svr_map; //port 
mutex_locker svr_locker;

TcpServer *findptr(unsigned short port)
{
	TcpServer * ptcpsvr = NULL;

	svr_locker.mutex_lock();	
	map<unsigned short,TcpServer*>::iterator iter;
	iter = svr_map.find(port);
	if(iter != svr_map.end())
	{
		//printf("svraddr exist\n");
		ptcpsvr = iter->second;
	}
	else
	{
		//printf("not found port\n");				
	}	
	svr_locker.mutex_unlock();
	return ptcpsvr;
}
void insertptr(unsigned short port,TcpServer *ptcpsvr)
{	
	svr_locker.mutex_lock();
	svr_map.insert(make_pair(port,ptcpsvr));
	svr_locker.mutex_unlock();	
}
void clearptr(unsigned short port)
{
	TcpServer * ptcpsvr = NULL;

	svr_locker.mutex_lock();
	map<unsigned short,TcpServer*>::iterator iter;
	iter = svr_map.find(port);
	if(iter != svr_map.end())
	{
		//printf("svraddr exist\n");
		ptcpsvr = iter->second;
		//printf("pfac = %p\n",pfac);
		svr_map.erase(iter);
		if(ptcpsvr != NULL)
		{
			delete ptcpsvr;
			ptcpsvr = NULL;
		}
		//printf("---------------\n");
	}
	else
	{
		//printf("not found port\n");			
	}	
	svr_locker.mutex_unlock();	
}

int StartTCPService(unsigned short svrport,ptcpFun Callback,pNotifyFun notifyCallback,pReadPacketFun readpacket)
{
	TcpServer *ptcpserver = NULL;
	ptcpserver = findptr(svrport);
	if(NULL == ptcpserver)
	{
		//printf("ptcpserver == NULL.\n");
		ptcpserver = new TcpServer(THREAD_NUM);
	}
	if(NULL == ptcpserver)
	{
		//printf("TcpServer new failed.\n");
		return -1;
	}
	insertptr(svrport,ptcpserver);
	if(ptcpserver->init(svrport,Callback,notifyCallback,readpacket))
	{
		if(ptcpserver->startpool())
		{
			ptcpserver->epoll();	
		}	
	}
	else
	{
		//printf("TcpServer init failed.\n");
		clearptr(svrport);
		ptcpserver = NULL;
		return -1;		
	}
	return 0;
}
unsigned long GetTCPClientTime(unsigned short svrport,int fd)
{
	TcpServer *ptcpserver = NULL;
	unsigned long timestamp;
	ptcpserver = findptr(svrport);
	if(NULL == ptcpserver)
	{
		//printf("ptcpserver == NULL.\n");
		return 0;
	}
	timestamp = ptcpserver->getclienttime(fd);
	return timestamp;
}
void DisconnectClient(unsigned short svrport,int fd)
{
	TcpServer *ptcpserver = NULL;
	ptcpserver = findptr(svrport);
	if(NULL == ptcpserver)
	{
		//printf("ptcpserver == NULL.\n");
		return;
	}
	ptcpserver->disconnect(fd);
}

