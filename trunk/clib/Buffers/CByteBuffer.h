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

#include "../../Defines.h"

#ifdef INTEL_SCALABLE_ALLOCATOR
    #include "tbb/scalable_allocator.h"
    #define _CALLOC(num,size)   scalable_calloc(num, size)
    #define _REALLOC(ptr,size)  scalable_realloc(ptr, size)
    #define _FREE(ptr)          scalable_free(ptr)
#else
    #define _CALLOC(num,size)   calloc(num, size)
    #define _REALLOC(ptr,size)  realloc(ptr, size)
    #define _FREE(ptr)          free(ptr)
#endif
    
typedef struct CByteBuffer
{
    uint8*  storage;
    size_t  size;
    size_t  capacity;
	size_t  rpos;
	size_t  wpos;
} bbuff;

/** Create bytebuffer, storage is not allocated
 */
#define bbuff_create(self)                                                              \
    do                                                                                  \
    {                                                                                   \
        self = (bbuff*)_CALLOC(1, sizeof(bbuff));                                       \
    }while(0)

/** Destroy bytebuffer and free storage
 */
#define bbuff_destroy(self)                                                             \
    do                                                                                  \
    {                                                                                   \
        _FREE((self)->storage);                                                         \
        _FREE(self);                                                                    \
    }while(0)
    
    
/** Reserve space in storage
 */
#define bbuff_reserve(self, ressize)                                                    \
    do                                                                                  \
    {                                                                                   \
        if(ressize > (self)->capacity)                                                  \
        {                                                                               \
            (self)->capacity = ressize;                                                 \
            if(((self)->storage = (uint8*)_REALLOC((self)->storage, ressize)) == NULL)  \
            {                                                                           \
                assert(false);                                                          \
            }                                                                           \
        }                                                                               \
    }while(0)
    
/** Resize storage
 */
#define bbuff_resize(self, newsize)                                                     \
    do                                                                                  \
    {                                                                                   \
        bbuff_reserve(self, newsize);                                                   \
        (self)->size = newsize;                                                         \
        (self)->rpos = 0;                                                               \
        (self)->wpos = (self)->size;                                                    \
    }while(0)

/** Append data to bytebuffer
 */
#define bbuff_append(self, src, len)                                                    \
    do                                                                                  \
    {                                                                                   \
        if(len > 0)                                                                     \
        {                                                                               \
            if((self)->size < ((self)->wpos + len))                                     \
            {                                                                           \
                bbuff_reserve(self, (self)->wpos + len);                                \
                (self)->size = (self)->wpos + len;                                      \
            }                                                                           \
            memcpy((self)->storage + (self)->wpos, src, len);                           \
            (self)->wpos += len;                                                        \
        }                                                                               \
    }while(0)
   
/** Read data from bytebuffer
 */
#define bbuff_read(self, dst, len)                                                      \
    do                                                                                  \
    {                                                                                   \
        if(((self)->rpos + len) <= (self)->size)                                        \
        {                                                                               \
            memcpy(dst, (self)->storage + (self)->rpos, len);                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            memset(dst, 0, len);                                                        \
        }                                                                               \
        (self)->rpos += len;                                                            \
    }while(0)
    
/** Put data to bytebuffer to specified position
 */
#define bbuff_put(self, pos, src, len)                                                  \
    do                                                                                  \
    {                                                                                   \
        assert(pos + len <= (self)->size);                                              \
        memcpy((self)->storage + pos, src, len);                                        \
    }while(0)
    
/** Clear bytebuffer data
 */
#define bbuff_clear(self)                                                               \
    do                                                                                  \
    {                                                                                   \
        _FREE((self)->storage);                                                         \
        memset(self, 0, sizeof(bbuff));                                                 \
    }while(0)
    
#ifdef __cplusplus
}
#endif

#endif
