/******************************************************************
* Copyright(c) 2020-2028 ZHENGZHOU Tiamaes LTD.
* All right reserved. See COPYRIGHT for detailed Information.
*
* @fileName: RingBuffer.cpp
* @brief: 环形队列实现
* @author: dinglf
* @date: 2020-05-06
* @history:
*******************************************************************/

#include "RingBuffer.h"

bool TRingBuffer::Create(int32 iBufSize )
{
    bool bResult = false;

    m_pBuf = new uint8[iBufSize];
    if( m_pBuf )
    {
        m_nBufSize = iBufSize;
        memset(m_pBuf, 0, m_nBufSize);

        //m_pTmpBuf = new char[ iBufSize ];
        //if( m_pTmpBuf )
        {
            //memset( m_pTmpBuf, 0, m_nBufSize);
            bResult = true;
        }
    }

    return bResult;
}

void TRingBuffer::Clear()
{
    m_iReadPtr = m_iWritePtr;
}


int32 TRingBuffer::GetMaxReadSize()
{
    if( m_pBuf )
    {
        if( m_iReadPtr == m_iWritePtr )
            return 0;

        if( m_iReadPtr < m_iWritePtr )
            return m_iWritePtr - m_iReadPtr;

        if( m_iReadPtr > m_iWritePtr )
            return (m_nBufSize-m_iReadPtr)+m_iWritePtr;
    }
    return 0;
}


int32 TRingBuffer::GetMaxWriteSize()
{
    if( m_pBuf )
    {
        if( m_iReadPtr == m_iWritePtr )
            return m_nBufSize;

        if( m_iWritePtr < m_iReadPtr )
            return m_iReadPtr - m_iWritePtr;

        if( m_iWritePtr > m_iReadPtr )
            return (m_nBufSize-m_iWritePtr)+m_iReadPtr;
    }
    return 0;
}


bool TRingBuffer::WriteBinary(uint8* pBuf, int32 nBufLen )
{
    bool bResult = false;

    if( nBufLen <= GetMaxWriteSize() )
    {
        // easy case, no wrapping
        if( m_iWritePtr + nBufLen <= m_nBufSize )
        {
            memcpy( &m_pBuf[m_iWritePtr], pBuf, nBufLen );
            m_iWritePtr += nBufLen;
        }
        else // harder case we need to wrap
        {
            int iFirstChunkSize = m_nBufSize - m_iWritePtr;

            int iSecondChunkSize = nBufLen - iFirstChunkSize;

            memcpy( &m_pBuf[m_iWritePtr], pBuf, iFirstChunkSize );
            memcpy( &m_pBuf[0], &pBuf[iFirstChunkSize], iSecondChunkSize );

            m_iWritePtr = iSecondChunkSize;
        }

        bResult = true;
    }

    return bResult;
}


bool TRingBuffer::ReadBinary(uint8* pBuf, int32 nBufLen )
{
    bool bResult = false;

    if( nBufLen <= GetMaxReadSize() )
    {
        // easy case, no wrapping
        if( m_iReadPtr + nBufLen <= m_nBufSize )
        {
            memcpy( pBuf, &m_pBuf[m_iReadPtr], nBufLen );
            m_iReadPtr += nBufLen;
        }
        else // harder case, buffer wraps
        {
            int iFirstChunkSize = m_nBufSize - m_iReadPtr;
            int iSecondChunkSize = nBufLen - iFirstChunkSize;

            memcpy( pBuf, &m_pBuf[m_iReadPtr], iFirstChunkSize );
            memcpy( &pBuf[iFirstChunkSize], &m_pBuf[0], iSecondChunkSize );

            m_iReadPtr = iSecondChunkSize;
        }
        bResult = true;
    }

    return bResult;
}


bool TRingBuffer::GetBytes(uint8* pBuf, int32 iGetPos,int32 nBufLen)
{
    bool bResult = false;

    if( nBufLen <= GetMaxReadSize() )
    {
        // easy case, no wrapping
        if( m_iReadPtr + nBufLen <= m_nBufSize )
        {
            memcpy( pBuf, &m_pBuf[m_iReadPtr], nBufLen );
        }
        else // harder case, buffer wraps
        {
            int iFirstChunkSize = m_nBufSize - m_iReadPtr;
            int iSecondChunkSize = nBufLen - iFirstChunkSize;

            memcpy( pBuf, &m_pBuf[m_iReadPtr], iFirstChunkSize );
            memcpy( &pBuf[iFirstChunkSize], &m_pBuf[0], iSecondChunkSize );
        }
        bResult = true;
    }



    /*if(m_iReadPtr<m_iWritePtr)
    {
        if(m_iReadPtr+nBufLen<=m_iWritePtr)
        {
            memcpy( pBuf, &m_pBuf[m_iReadPtr], nBufLen);
            bResult = true;
        }
    }
    else
    {
        if( m_iReadPtr + nBufLen <= m_nBufSize )
        {
            memcpy( pBuf, &m_pBuf[m_iReadPtr], nBufLen );
        }
        else // harder case, buffer wraps
        {
            int iFirstChunkSize = m_nBufSize - m_iReadPtr;
            int iSecondChunkSize = nBufLen - iFirstChunkSize;

            memcpy( pBuf, &m_pBuf[m_iReadPtr], iFirstChunkSize );
            memcpy( &pBuf[iFirstChunkSize], &m_pBuf[0], iSecondChunkSize );
        }
    }*/

    return bResult;
}



bool TRingBuffer::PeekChar(int32 iPos, uint8 & ch )
{
    bool bResult = false;

    if( iPos < GetMaxReadSize() )
    {
        if( m_iWritePtr > m_iReadPtr )
        {
            // easy case, buffer doesn't wrap
            ch = m_pBuf[ m_iReadPtr+iPos ];
            bResult = true;
        }
//        else if( m_iWritePtr == m_iReadPtr )
//        {
//            // nothing in the buffer
//        }
        else //if( m_iWritePtr < m_iReadPtr )
        {
            // harder case, buffer wraps

            if( m_iReadPtr + iPos < m_nBufSize )
            {
                // pos was in first chunk
                ch = m_pBuf[ m_iReadPtr + iPos ];
            }
            else
            {
                // pos is in second chunk
                ch = m_pBuf[ iPos - (m_nBufSize-m_iReadPtr) ];
            }
            bResult = true;
        }
    }

    return bResult;
}


bool TRingBuffer::FindChar(uint8 chLookFor, int32& riPos )
{
    bool bResult = false;

    int iSize = GetMaxReadSize();

    for( int32 i = 0; i < iSize; i++ )
    {
        uint8 ch;
        if( PeekChar( i, ch ) )
        {
            if( ch == chLookFor )
            {
                riPos = i;
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}
