//
//  CByteBuffer.c
//  TransDB
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CByteBuffer.h"

CByteBuffer *CByteBuffer_create()
{
    CByteBuffer *pCByteBuffer = calloc(1, sizeof(CByteBuffer));
    return pCByteBuffer;
}

void CByteBuffer_destroy(CByteBuffer *pCByteBuffer)
{
    free(pCByteBuffer->m_storage);
    free(pCByteBuffer);
}

void CByteBuffer_reserve(CByteBuffer *pCByteBuffer, size_t newCapacity)
{
    assert(newCapacity >= 0);
    
    if(newCapacity > pCByteBuffer->m_capacity)
    {
        //set capacity
        pCByteBuffer->m_capacity = newCapacity;
        //resize buffer
        void *pNewBuffer = realloc(pCByteBuffer->m_storage, newCapacity);
        if(pNewBuffer == NULL)
            return;
        
        pCByteBuffer->m_storage = pNewBuffer;
    }
}

void CByteBuffer_resize(CByteBuffer *pCByteBuffer, size_t newSize)
{
    CByteBuffer_reserve(pCByteBuffer, newSize);
    pCByteBuffer->m_size = newSize;
    pCByteBuffer->m_rpos = 0;
    pCByteBuffer->m_wpos = pCByteBuffer->m_size;
}

void CByteBuffer_append(CByteBuffer *pCByteBuffer, const void *pSource, size_t srcSize)
{
    if(srcSize == 0)
        return;
    
    if(pCByteBuffer->m_size < (pCByteBuffer->m_wpos + srcSize))
    {
        CByteBuffer_reserve(pCByteBuffer, pCByteBuffer->m_wpos + srcSize);
        pCByteBuffer->m_size = pCByteBuffer->m_wpos + srcSize;
    }
    
    memcpy(pCByteBuffer->m_storage + pCByteBuffer->m_wpos, pSource, srcSize);
    pCByteBuffer->m_wpos += srcSize;
}

void CByteBuffer_read(CByteBuffer *pCByteBuffer, void *pDestination, size_t len)
{
    if((pCByteBuffer->m_rpos + len) <= pCByteBuffer->m_size)
    {
        memcpy(pDestination, pCByteBuffer->m_storage + pCByteBuffer->m_rpos, len);
    }
    else
    {
        memset(pDestination, 0, len);
    }
    pCByteBuffer->m_rpos += len;
}

void CByteBuffer_put(CByteBuffer *pCByteBuffer, size_t pos, const void *pSource, size_t srcSize)
{
    assert(pos + srcSize <= pCByteBuffer->m_size);
    memcpy(pCByteBuffer->m_storage + pos, pSource, srcSize);
}





