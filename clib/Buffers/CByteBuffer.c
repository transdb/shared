//
//  CByteBuffer.c
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#include "CByteBuffer.h"

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

bbuff *bbuff_create(void)
{
    bbuff *p = _CALLOC(1, sizeof(bbuff));
    assert(p);
    return p;
}

void bbuff_destroy(bbuff *self)
{
    _FREE(self->storage);
    _FREE(self);
}

void bbuff_reserve(bbuff *self, size_t ressize)
{ 
	//for WP8 - C89
	void *pNewBuffer;

    if(ressize > self->capacity)
    {
        //set capacity
        self->capacity = ressize;
        //resize buffer
        pNewBuffer = _REALLOC(self->storage, ressize);
        if(pNewBuffer == NULL)
        {
            assert(false);
            return;
        }
        self->storage = pNewBuffer;
    }
}

void bbuff_resize(bbuff *self, size_t newsize)
{
    bbuff_reserve(self, newsize);
    self->size = newsize;
    self->rpos = 0;
    self->wpos = self->size;
}

void bbuff_append(bbuff *self, const void *src, size_t len)
{
    if(len > 0)
    {
        if(self->size < (self->wpos + len))
        {
            bbuff_reserve(self, self->wpos + len);
            self->size = self->wpos + len;
        }
        
        memcpy(self->storage + self->wpos, src, len);
        self->wpos += len;
    }
}

void bbuff_read(bbuff *self, void *dst, size_t len)
{
    if((self->rpos + len) <= self->size)
    {
        memcpy(dst, self->storage + self->rpos, len);
    }
    else
    {
        memset(dst, 0, len);
    }
    self->rpos += len;
}

void bbuff_put(bbuff *self, size_t pos, const void *src, size_t len)
{
    assert(pos + len <= self->size);
    memcpy(self->storage + pos, src, len);
}

void bbuff_clear(bbuff* self)
{
    _FREE(self->storage);
    memset(self, 0, sizeof(bbuff));
}




