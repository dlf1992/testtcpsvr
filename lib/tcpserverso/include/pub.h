/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: pub.h
* @brief: 公共函数头定义
* @author: dinglf
* @date: 2020-06-05
* @history:
*******************************************************************/
#ifndef _PUB_H_
#define _PUB_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <math.h>
#include "RingBuffer.h"

using namespace std;
enum ret { kvalid=0,kinvalid };    // 是否有非法输入的标记
#ifdef __cplusplus
extern "C"  //C++
{
#endif
	//系统UTC时间戳函数
	extern unsigned long GetUtcTime();
	//系统启动时间戳函数
	extern unsigned long GetSysTime();
	//计算crc16
	extern uint16 crc16_xmodem(uint8 *data, uint32 length); 
	//计算crc32
	extern uint32 crc32(uint8 *data, uint32 length);
	//SYS_CRC16
	extern uint16 SYS_CRC16(const char *dt, uint16 len);
	//字符串分割
	extern vector<string> split(const string& src, string separate_character);
	//字符串转换整型
	extern int Strtoint(const char* str);
	//shell命令获取结果
	extern int run_sh_cmd(const char *cmd,char *result);
	//十六进制转换十进制
	extern int hex_to_decimal(const char* szHex, int len);
	//查看进程是否运行
	extern bool ProcessIsRun(const char *proc);
	//读文件到string
	extern int readfile(const char *filename,string &strinfo);
	//执行shell命令
	extern int execl_sh_cmd(const char *cmd);
#ifdef __cplusplus
}
#endif
#endif