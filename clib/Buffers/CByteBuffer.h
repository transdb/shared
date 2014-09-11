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

typedef struct CByteBuffer
{
    uint8   *storage;
    size_t  size;
    size_t  capacity;
	size_t  rpos;
	size_t  wpos;
} bbuff;

/** Create bytebuffer, storage is not allocated
 */
bbuff *bbuff_create(void);

/** Destroy bytebuffer and free storage
 */
void bbuff_destroy(bbuff *bbuff);
    
/** Reserve space in storage
 */
void bbuff_reserve(bbuff *bbuff, size_t ressize);
    
/** Resize storage
 */
void bbuff_resize(bbuff *bbuff, size_t newsize);

/** Append data to bytebuffer
 */
void bbuff_append(bbuff *bbuff, const void *src, size_t len);
   
/** Read data from bytebuffer
 */
void bbuff_read(bbuff *bbuff, void *dst, size_t len);
    
/** Put data to bytebuffer to specified position
 */
void bbuff_put(bbuff *bbuff, size_t pos, const void *src, size_t len);
    
#ifdef __cplusplus
}
#endif

#endif
