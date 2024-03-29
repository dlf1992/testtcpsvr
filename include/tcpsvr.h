/*
 * 	tcpsvr.h
 *
 */
 
/*条件编译*/
#ifndef TCP_SVR_H_
#define TCP_SVR_H_
//#include <iostream>
//#include <map>
//using namespace std;
#include "RingBuffer.h"

#ifdef __cplusplus
extern "C"  //C++
{
#endif
typedef  int (*ptcpFun)(const char *,int,int);
typedef  int (*pNotifyFun)(const char *,int);
typedef  int (*pReadPacketFun)(TRingBuffer*,char*,int);
int StartTCPService(unsigned short svrport,ptcpFun Callback,pNotifyFun notifyCallback,pReadPacketFun readpacket);
unsigned long GetTCPClientTime(unsigned short svrport,int fd);
void DisconnectClient(unsigned short svrport,int fd);
#ifdef __cplusplus
}
#endif
 
#endif /* TCP_SVR_H_ */