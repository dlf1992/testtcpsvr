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
 
#ifdef __cplusplus
extern "C"  //C++
{
#endif
typedef  int (*ptcpFun)(const char *,int,int);
typedef  int (*pNotifyFun)(const char *,int);
int StartTCPService(unsigned short svrport,ptcpFun Callback,pNotifyFun notifyCallback);
#ifdef __cplusplus
}
#endif
 
#endif /* TCP_SVR_H_ */