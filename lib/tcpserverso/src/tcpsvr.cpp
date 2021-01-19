#include <stdio.h>
#include <stdlib.h>
#include "tcpserver.h"
#include "tcpsvr.h"
#include "taskprocess.h"

int StartTCPService(unsigned short svrport,ptcpFun Callback,pNotifyFun notifyCallback)
{
	TcpServer * ptcpserver = TcpServer::GetInstance();
	if(NULL == ptcpserver)
	{
		//printf("ptcpserver == NULL.\n");
		return -1;
	}
	if(ptcpserver->init(svrport,Callback,notifyCallback))
	{
		if(ptcpserver->startpool())
		{
			ptcpserver->epoll();	
		}	
	}
	else
	{
		//printf("TcpServer init failed.\n");
		TcpServer::Destroy();
		return -1;		
	}
	return 0;
}
