/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: cserial.cpp
* @brief: 公共函数定义
* @author: dinglf
* @date: 2020-06-05
* @history:
*******************************************************************/
#include "pub.h"
int strtointstatus = kvalid;
//CRC16表格
const static uint16 crc16_table[256]=
{
    0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
    0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
    0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
    0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
    0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
    0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
    0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
    0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
    0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
    0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
    0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
    0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
    0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
    0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
    0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
    0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
    0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
    0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
    0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
    0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
    0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
    0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
    0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
    0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
    0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
    0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
    0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
    0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
    0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
    0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
    0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
    0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

/*
 * 函数名：GetUtcTime
 * 功能：获取Utc时间戳,系统的时间ms
 * 形参：
 * 返回值：unsigned long
*/
unsigned long GetUtcTime()
{
	unsigned long timestamp = 0;
	struct timespec time = {0, 0};
	clock_gettime(CLOCK_REALTIME, &time);
	timestamp = time.tv_sec*1000+time.tv_nsec/1000000;
	return timestamp;
}

/*
 * 函数名：GetSysTime
 * 功能：获取系统时间戳,系统启动的时间ms
 * 形参：
 * 返回值：unsigned long
*/
unsigned long GetSysTime()
{
	unsigned long timestamp = 0;
	struct timespec time = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &time);
	timestamp = time.tv_sec*1000+time.tv_nsec/1000000;
	return timestamp;
}
/*
	功能：计算crc16，采用Crc16-XModem标准
	形参：data是输入数据内容，length是数据长度
	返回值：crc
*/
uint16 crc16_xmodem(uint8 *data, uint32 length)  
{  
    uint8 i;  
    uint16 crc = 0;       // Initial value  
    while(length--)  
    {  
        crc ^= (uint16)(*data++) << 8; // crc ^= (uint16)(*data)<<8; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if ( crc & 0x8000 )  
                crc = (crc << 1) ^ 0x1021;  
            else  
                crc <<= 1;  
        }  
    }  
    return crc;  
}  
/*
	功能：计算crc32，采用IEEE802标准
	形参：data是输入数据内容，length是数据长度
	返回值：crc
*/
uint32 crc32(uint8 *data, uint32 length)  
{  
    uint8 i;  
    uint32 crc = 0xffffffff;  // Initial value  
    while(length--)  
    {  
        crc ^= *data++;         // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return ~crc;  
}
//CRC16
uint16 SYS_CRC16(const char *dt, uint16 len)
{
    uint16 i;
    uint16 res;
    for (i=0; i<len; i++){
        if (dt[i]!=0){
            res=0;
            for (i=0; i<len; i++){
                res=(crc16_table[dt[i]^(res&0xFF)]^(res/0x100));
            }
            //res = (res>>8) |(res<<8); ///适应小端模式
            return (res);
        }
    }
    return (0xFFFF);
}
//字符串分割
vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;
	int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
	if (!lastString.empty())
		strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
	return strs;
}
long long Strtointcode(const char* digit, bool minus)
{
    long long num = 0;
    while (*digit != '\0')
    {
        if (*digit >= '0'&&*digit <= '9')
        {
            int flag = minus ? -1 : 1;
            num = num * 10 + flag*(*digit - '0');
            if ((!minus&&num > 0x7FFFFFFF) ||( minus&&num < (signed int)0x80000000))
            {
                num = 0;
                break;
            }
            digit++;
        }
        else
        {
            num = 0;
            break;
        }
    }
    if (*digit == '\0')
        strtointstatus = kvalid;
    return num;
}
/*
 * 功能：字符串转换int
 * 形参：str输入字符串
 * 返回值：输出int值
 */
int Strtoint(const char* str)
{
    strtointstatus = kinvalid;
    long long num = 0;
    if (str != NULL&&*str != '\0')
    {
        bool minus = false;
        if (*str == '+')
            str++;
        else if (*str == '-')
        {
            str++;
            minus = true;
        }
        if (*str != '\0')
            num = Strtointcode(str, minus);

    }
    return (int)num;
}
/*
* 功能：shell命令获取结果
* 形参：shell命令、结果
* 返回值：
*/
int run_sh_cmd(const char *cmd,char *result)
{
	FILE *pfd1 =NULL;
	
	int ret = -1;
	char cmd_result_buf[1024];
	memset(cmd_result_buf, 0, sizeof(cmd_result_buf));
	pfd1 = popen(cmd, "r");
	if(!pfd1)
	{
		//printf("%s\n",cmd);
		return -1;
	}
	ret = fread(cmd_result_buf, sizeof(char),sizeof(cmd_result_buf)-1, pfd1);
	if(ret <= 0)
	{
		//printf("read_cpe_config error\n");
		pclose(pfd1);
		pfd1 = NULL;
		return -1;
	}
	//cmd_result_buf[ret-1] = '\0';
	////printf("cmd_result_buf:%s\n",cmd_result_buf);
	memcpy(result,cmd_result_buf,strlen(cmd_result_buf));
	pclose(pfd1);
	pfd1 = NULL;
	return 0;
}
int hex_char_value(char c)
{
	if(c >= '0' && c <= '9')
	{	
		return c - '0';
	}
	else if(c >= 'a' && c <= 'f')
	{	
		return (c - 'a' + 10);
	}
	else if(c >= 'A' && c <= 'F')
	{
		return (c - 'A' + 10);
	}
	//assert(0);
	return 0;
}
int hex_to_decimal(const char* szHex, int len)
{
	int result = 0;
	for(int i = 0; i < len; i++)
	{
		result += (int)pow((float)16, (int)len-i-1) * hex_char_value(szHex[i]);
	}
	return result;
}
/*
* 功能：根据进程名字查询是否运行
* 形参：进程名字
* 返回值：true 运行 false 未运行
*/
bool ProcessIsRun(const char *proc)
{
    FILE* fp = NULL; 
    int count = 1; 
    int BUFSZ = 100; 
    char buf[BUFSZ]; 
    char command[150]; 

	memset(command,0,sizeof(command));
    sprintf(command, "ps -ef | grep -w \'%s\' | grep -v grep | wc -l", proc); 
	//printf("command:%s\n",command);
    if((fp = popen(command,"r")) == NULL) 
    { 
        //LogError("popen err");
        return false;
    } 
    if((fgets(buf,BUFSZ,fp))!= NULL) 
    { 
        count = atoi(buf); 
		//printf("buf:%s\n",buf);
    } 
    
    pclose(fp); 
    
    fp=NULL; 
    
    if ((count-1) == 0) 
	{
        return false;
    } 
	else 
	{
        return true;
    }
}
/*
* 功能：读文件到string
* 形参：
* 返回值：
*/
int readfile(const char *filename,string &strinfo)
{
	FILE *fp;
	int len;
	char *pbuf = NULL;
	
	fp = fopen(filename, "r");	//前提该文件必须存在
	if(fp == NULL ) 
	{
	  //perror("打开文件错误");
	  return -1;
	}
	fseek(fp, 0, SEEK_END);

	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(len == 0)
	{
		len = 2048;
	}	

	//申请内存
	pbuf= (char *) malloc(len+1);
	memset(pbuf,0,len+1);
	/* 读取并显示数据 */
	fread(pbuf, len, 1, fp);
	//printf("show: %s.\n", pbuf);
	strinfo = pbuf;
	fclose(fp);
	free(pbuf);	
	return 0;
}
/*
* 功能：执行shell命令
* 形参：shell命令
* 返回值：
*/
int execl_sh_cmd(const char *cmd)
{
	FILE *pfd1 =NULL;
	pfd1 = popen(cmd, "r");
	if(!pfd1)
	{
		//printf("%s\n",cmd);
		return -1;
	}
	pclose(pfd1);
	pfd1 = NULL;
	return 0;
}


