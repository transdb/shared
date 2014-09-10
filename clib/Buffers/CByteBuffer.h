//
//  CByteBuffer.h
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef CBYTEBUFFER_H
#define CBYTEBUFFER_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "../CDefines.h"

typedef struct
{
    uint8   *m_storage;
    size_t  m_size;
    size_t  m_capacity;
	size_t  m_rpos;
	size_t  m_wpos;
} CByteBuffer;

/**
 */
CByteBuffer *CByteBuffer_create();

/**
 */
void CByteBuffer_destroy(CByteBuffer *pCByteBuffer);
    
/**
 */
void CByteBuffer_reserve(CByteBuffer *pCByteBuffer, size_t ressize);
    
/**
 */
void CByteBuffer_resize(CByteBuffer *pCByteBuffer, size_t newsize);

/** 
 */
void CByteBuffer_append(CByteBuffer *pCByteBuffer, const void *pSource, size_t srcSize);
   
/**
 */
void CByteBuffer_read(CByteBuffer *pCByteBuffer, void *pDestination, size_t len);
    
/**
 */
void CByteBuffer_put(CByteBuffer *pCByteBuffer, size_t pos, const void *pSource, size_t srcSize);
    
#ifdef __cplusplus
}
#endif

#endif
