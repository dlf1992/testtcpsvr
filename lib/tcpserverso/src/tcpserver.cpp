/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: tcpserver.cpp
* @brief: TCP通信服务端实现
* @author: dinglf
* @date: 2020-12-19
* @history:
*******************************************************************/
#include "tcpserver.h"

bool TcpServer::init(unsigned short svrport,ptcpFun callback,pNotifyFun notifycallback,pReadPacketFun readpacket)
{
	m_port = svrport;
	m_notifyfunc = notifycallback;
	pclient_buffer = new CientBuffer(m_port);
	if(NULL == pclient_buffer)
	{
		//printf("pclient_buffer NULL,CientBuffer new failed.\n");
		return false;
	}
	pclient_buffer->task_callback = callback;
	pclient_buffer->read_packet = readpacket;
	bzero(&ServerAddr, sizeof(ServerAddr));
	//配置ServerAddr
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(m_port);
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //创建Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        //printf("TcpServer socket init error\n");
        return false;
    }
    int opt = 1;
    // sockfd为需要端口复用的套接字
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, (socklen_t)sizeof(opt));

	//获取发送缓冲区
    int sendbuf_len;
    int len = sizeof(sendbuf_len);
    if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&sendbuf_len, (socklen_t*)&len) < 0)
	{
        //printf("getsockopt: error\n");
        return false;
    }
	//printf("sendbuf_len = %d\n",sendbuf_len);
	//设置发送缓冲区
	sendbuf_len *= 2;  //发送缓冲区翻倍，实际缓冲区大小的一半
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *)&sendbuf_len, (socklen_t)len) < 0)
	{
        //printf("setsockopt:error\n");
        return false;
    }
	int keepAlive = 1;	  // 非0值，开启keepalive属性
	int keepIdle = 60;	  // 如该连接在60秒内没有任何数据往来,则进行此TCP层的探测
	int keepInterval = 5; // 探测发包间隔为5秒
	int keepCount = 3;		  // 尝试探测的最多次数
	// 开启探活
	
	if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive)))
	{
		//printf("set socket KEEPALIVE error.\n");
		close(sockfd);
		return false;
	}
	if(-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
	{
		printf("set socket KEEPIDLE error.\n");
		close(sockfd);
		return false;
	}
	if(-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)))
	{
		//printf("set socket KEEPINTVL error.\n");
		close(sockfd);
		return false;		
	}
	if(-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)))
	{
		//printf("set socket KEEPCNT error.\n");
		close(sockfd);
		return false;
	}	
    int ret = bind(sockfd, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr));
    if(ret < 0)
    {
        //printf("TcpServer bind init error\n");
        close(sockfd);
        return false;
    }
    ret = listen(sockfd, 10);
    if(ret < 0)
    {
        //printf("TcpServer listen init error\n");
        close(sockfd);
        return false;
    }
    //create Epoll
    epollfd = epoll_create(1024);
    if(epollfd < 0)
    {
        //printf("TcpServer epoll_create init error\n");
        close(sockfd);
        return false;
    }
	//printf("TcpServer init success.\n");
	return true;
    //pool = new threadpool<BaseTask>(threadnum);  //创建线程池	
}
bool TcpServer::startpool()
{
    pool = new threadpool<BaseTask>(threadnum);  //创建线程池
	if(NULL == pool)
	{
		//printf("pool=NULL.\n");
		close(sockfd);
		return false;
	}
    pool->start();   //线程池启动
	return true;
}
void TcpServer::addclienttime(int fd,const char* clientinfo) 
{
	unsigned long timestamp;
	timestamp = GetSysTime();
	string client  = clientinfo;
	activeconnectmaplocker.mutex_lock();
	m_activeclient.insert(pair<int,unsigned long>(fd,timestamp));
	m_connecttime.insert(pair<int,unsigned long>(fd,timestamp));
	m_clientinfo.insert(pair<int,string>(fd,client));
	activeconnectmaplocker.mutex_unlock();	
}
void TcpServer::removeclienttime(int fd) 
{
	activeconnectmaplocker.mutex_lock();
	map<int,unsigned long>::iterator iter;
	iter = m_activeclient.find(fd);
	if(iter != m_activeclient.end())
	{
		m_activeclient.erase(iter);
	}
	map<int,unsigned long>::iterator iter1;
	iter1 = m_connecttime.find(fd);
	if(iter1 != m_connecttime.end())
	{
		m_connecttime.erase(iter1);
	}
	map<int,string>::iterator iter2;
	iter2 = m_clientinfo.find(fd);
	if(iter2 != m_clientinfo.end())
	{
		m_clientinfo.erase(iter2);
	}	
	activeconnectmaplocker.mutex_unlock();	
}
void TcpServer::clearclienttime()
{
	activeconnectmaplocker.mutex_lock();
	map<int,unsigned long>::iterator iter;
	for(iter=m_activeclient.begin();iter!=m_activeclient.end();)
	{
		m_activeclient.erase(iter++);
	}
	map<int,unsigned long>::iterator iter1;
	for(iter1=m_connecttime.begin();iter1!=m_connecttime.end();)
	{
		m_connecttime.erase(iter1++);
	}
	map<int,string>::iterator iter2;
	for(iter2=m_clientinfo.begin();iter2!=m_clientinfo.end();)
	{
		m_clientinfo.erase(iter2++);
	}	
	activeconnectmaplocker.mutex_unlock();		
}
void TcpServer::updateclienttime(int fd) 
{
	unsigned long timestamp;
	timestamp = GetSysTime();
	activeconnectmaplocker.mutex_lock();
	map<int,unsigned long>::iterator iter;
	iter = m_activeclient.find(fd);
	if(iter != m_activeclient.end())
	{
		m_activeclient[fd] = timestamp;
	}
	else
	{
		m_activeclient.insert(pair<int,unsigned long>(fd,timestamp));
	}
	activeconnectmaplocker.mutex_unlock();	
}
void TcpServer::epoll()
{
    //pool->start();   //线程池启动
    char tmp[32];
	char notifydata[64];
    addfd(epollfd, sockfd, false);
    while(!is_stop)
    {//调用epoll_wait
        int ret = epoll_wait(epollfd, events, MAX_EVENT, -1);

        if(ret < 0)  //出错处理
        {
            //被信号中断
            if (errno == EINTR)
            {
                //printf("errno == EINTR\n");
                continue;
            }
            else
			{
                //printf("epoll_wait error\n");
                break;
            }
        }
        for(int i = 0; i < ret; ++i)
        {
            int fd = events[i].data.fd;
            if(fd == sockfd)  //新的连接到来
            {
				//printf("new client connect.\n");
                struct sockaddr_in clientAddr;
                socklen_t len = sizeof(clientAddr);
                int confd = accept(sockfd, (struct sockaddr *)
                        &clientAddr, &len);
                int port = ntohs(clientAddr.sin_port);
                struct in_addr in  = clientAddr.sin_addr;
                char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
                //成功的话此时IP地址保存在str字符串中。
                inet_ntop(AF_INET,&in, str, sizeof(str));
                //printf("client ip:port  %s : %d,confd: %d\n",str,port,confd);
                //printf("client confd: %d\n",confd);
				if(confd == -1)
				{
					//printf("errno = %d.\n",errno);
				}	
				addclienttime(confd,(const char*)str);
				if(m_notifyfunc != NULL)
				{
					memset(&m_eventnotify,0,sizeof(EVENTNOTIFY));
					m_eventnotify.cmd = 0x01;//连接
					m_eventnotify.fd = confd;
					memset(tmp,0,sizeof(tmp));
					sprintf(tmp,"%s:%d",str,port);
					memcpy(&m_eventnotify.clientinfo,tmp,32);
					memset(notifydata,0,sizeof(notifydata));
					memcpy(notifydata,&m_eventnotify,sizeof(EVENTNOTIFY));
					m_notifyfunc(notifydata,sizeof(EVENTNOTIFY));
				}
				if(m_port == 48003)
				{
					addfdext(epollfd, confd, false);
				}
				else
				{
                	addfd(epollfd, confd, false);
				}
            }
            else if(events[i].events & EPOLLIN)  //某个fd上有数据可读
            {
                char buffer[MAX_BUFFER];
                readagain:	memset(buffer, 0, sizeof(buffer));
                int ret1 = read(fd, buffer, MAX_BUFFER - 1);
                if(ret1 == 0)  //某个fd关闭了连接，从Epoll中删除并关闭fd
                {
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
                    shutdown(fd, SHUT_RDWR);
					close(fd);
                    //printf("%d logout\n", fd);
					removeclienttime(fd);
					if(m_notifyfunc != NULL)
					{
						memset(&m_eventnotify,0,sizeof(EVENTNOTIFY));
						m_eventnotify.cmd = 0x02;//断开
						m_eventnotify.fd = fd;
						memset(notifydata,0,sizeof(notifydata));
						memcpy(notifydata,&m_eventnotify,sizeof(EVENTNOTIFY));
						m_notifyfunc(notifydata,sizeof(EVENTNOTIFY));
					}
					if(pclient_buffer != NULL)
					{
						pclient_buffer->clearRingBuffer(fd);
					}
					else
					{
						//printf("clear fd,pclient_buffer = NULL\n");
					}
                    continue;
                }
                else if(ret1 < 0)//读取出错，尝试再次读取
                {
                    if(errno == EAGAIN)
                    {
                        //printf("read error! errno = EAGAIN,continue.\n");
                        continue;
                    }
					else if(errno == EINTR)
					{
                        //printf("read error! errno = EINTR,readagain.\n");
                        goto readagain;						
					}
					else 
					{
						//printf("read error! errno = %d,break.\n",errno);
						break;
					}
                }
                else//成功读取，向线程池中添加任务
                {
                    //printf("received data,fd = %d,datalen = %d\n",fd,ret1);
					BaseTask *task = NULL;
					if(NULL == pclient_buffer)
					{
						//printf("add task,pclient_buffer = NULL\n");
						break;
					}
                    task = new Task(pclient_buffer,buffer,ret1,fd);
					if(NULL == task)
					{
						//printf("task=NULL.\n");
						continue;
					}
					if(NULL == pool)
					{
						//printf("pool=NULL.\n");
						break;
					}
					////printf("epoll task = %p\n",task);
                    pool->append_task(task);
					updateclienttime(fd);
                }
            }
            else
            {
                //printf("something else had happened,fd = %d\n",fd);
            }
        }
    }
    close(sockfd);//结束。

    //pool->stop();
}
void TcpServer::disconnect(int fd)
{
	char notifydata[64];
	struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
    shutdown(fd, SHUT_RDWR);
    close(fd);
	removeclienttime(fd);
	if(m_notifyfunc != NULL)
	{
		memset(&m_eventnotify,0,sizeof(EVENTNOTIFY));
		m_eventnotify.cmd = 0x02;//断开
		m_eventnotify.fd = fd;
		memset(notifydata,0,sizeof(notifydata));
		memcpy(notifydata,&m_eventnotify,sizeof(EVENTNOTIFY));
		m_notifyfunc(notifydata,sizeof(EVENTNOTIFY));
	}	
	if(pclient_buffer != NULL)
	{
		pclient_buffer->clearRingBuffer(fd);
	}
    //printf("%d logout\n", fd);
}
void TcpServer::stoppool()
{
	if(NULL == pool)
	{
		//printf("pool=NULL.\n");
		return;
	}
    pool->stop();   //线程池停止	
}
int  TcpServer::senddata(int fd,const char *data,int datalen)
{
	bool  ret = false;
	activeconnectmaplocker.mutex_lock();
	map<int,string>::iterator iter;
	iter = m_clientinfo.find(fd);
	if(iter != m_clientinfo.end())
	{
		////printf("found fd,fd = %d\n",fd);
		ret = true;
	}
	else
	{
		ret = false;
	}
	activeconnectmaplocker.mutex_unlock();
	if(ret)
	{
		int send_len = 0;
		send_len = write(fd,data,datalen);
		return send_len;
	}
	else
	{
		return -1;
	}
}

unsigned long TcpServer::getclienttime(int fd)
{
	unsigned long timestamp = 0;
	activeconnectmaplocker.mutex_lock();
	map<int,unsigned long>::iterator iter;
	iter = m_activeclient.find(fd);
	if(iter != m_activeclient.end())
	{
		timestamp = m_activeclient[fd];
	}
	activeconnectmaplocker.mutex_unlock();
	return timestamp;
}