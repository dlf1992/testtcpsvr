/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: RingBuffer.h
* @brief: 环形队列
* @author: dinglf
* @date: 2020-05-06
* @history:
*******************************************************************/
#ifndef _TRINGBUFFER_H_
#define _TRINGBUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char  uint8;
typedef char 		   int8;
typedef unsigned short uint16;
typedef short 		   int16;
typedef unsigned int   uint32;
typedef int 		   int32;

#define MAKESHORT(a,b) ((a&0xff)|((b&0xff)<<8))
#define MAKEINT(a,b,c,d) ((a&0xff)|((b&0xff)<<8)|((c&0xff)<<16)|((d&0xff)<<24))
/*! @brief 环形队列类
 */
class  TRingBuffer
{
protected:

    //uint8* m_pTmpBuf;   //!<  临时指针



public:
    uint8* m_pBuf;      //!<  缓冲区指针
    int32 m_iReadPtr;   //!< 读指针
    int32 m_iWritePtr;  //!< 写指针
    int32 m_nBufSize;   //!< 环形队列的大小


public:
    /*! @brief 缺省构造函数 */
    TRingBuffer()
    {
        m_pBuf = (uint8*)(0);
        //m_pTmpBuf = (void*)(0);
        m_nBufSize = 0;
        m_iReadPtr = 0;
        m_iWritePtr = 0;
    }

    /*! @brief 析构函数 */
    virtual ~TRingBuffer()
    {
        Destroy();
    }

    /*! @brief 构造环形队列
      * @param  iBufSize  环形队列的最大长度
     * @return  TRUE 成功 其他 FALSE
      */
    bool Create(int32 iBufSize = 1024*3);


    /*! @brief 释放环形队列的空间 */
    void Destroy()
    {
        if( m_pBuf )
            delete []m_pBuf;

        //if( m_pTmpBuf )
        //     delete m_pTmpBuf;

        m_pBuf = 0;
        //m_pTmpBuf = 0;
        m_nBufSize = 0;
        m_iReadPtr = 0;
        m_iWritePtr = 0;
    }

    void Clear();


    /*! @brief 获取当前最多可读取的字节数
     *  @return 字节数
    */
    int32 GetMaxReadSize();



    /*! @brief 获取当前最多可写的字节数
     *  @return 字节数
    */
    int32 GetMaxWriteSize();


    /*! @brief 向环形队列中写入数据
     * @param pBuf 需要写入的数据缓冲区
     * @param nBufLen 写入数据的长度
     * @return 成功返回TRUE 失败返回FALSE
     */
    bool WriteBinary(uint8* pBuf, int32 nBufLen );


    /*! @brief 从环形队列中读出数据
     * @param  pBuf 数据存放的缓冲区指针
     * @param nBufLen 缓冲区长度
     * @return 成功返回TURE 否则返回FALSE
     */
    bool ReadBinary(uint8* pBuf, int32 nBufLen );



    bool GetBytes(uint8* pBuf, int32 iGetPos,int32 nBufLen);

    /*! @brief 从队列指定位置获取一个字节
     * @param iPos 获取字节的位置
     * @param ch 获取的字节
     * @return 成功返回TRUE 否则返回FALSE
     */
    bool PeekChar(int32 iPos, uint8 & ch );

    /*! @brief 在队列中查找字节
     * @param chLookFor 查找的字符
     * @param riPos 第一个找到字符位置
     * @return 成功返回TRUE 否则返回FALSE
     */
    bool FindChar(uint8 chLookFor, int32 & riPos);

    bool ThrowSomeData(int32 nBufLen )
    {
        if( nBufLen <= GetMaxReadSize() )
        {
            if (nBufLen > 0)
            {
                m_iReadPtr += nBufLen;
                m_iReadPtr %= m_nBufSize;
            }

            return true;
        }

        return false;
    }

};



#endif