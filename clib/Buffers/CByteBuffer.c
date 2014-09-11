//
//  CByteBuffer.c
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CByteBuffer.h"

bbuff *bbuff_create(void)
{
    bbuff *p = calloc(1, sizeof(bbuff));
    return p;
}

void bbuff_destroy(bbuff *bbuff)
{
    free(bbuff->storage);
    free(bbuff);
}

void bbuff_reserve(bbuff *bbuff, size_t ressize)
{
    assert(ressize >= 0);
    
    if(ressize > bbuff->capacity)
    {
        //set capacity
        bbuff->capacity = ressize;
        //resize buffer
        void *pNewBuffer = realloc(bbuff->storage, ressize);
        if(pNewBuffer == NULL)
            return;
        
        bbuff->storage = pNewBuffer;
    }
}

void bbuff_resize(bbuff *bbuff, size_t newsize)
{
    bbuff_reserve(bbuff, newsize);
    bbuff->size = newsize;
    bbuff->rpos = 0;
    bbuff->wpos = bbuff->size;
}

void bbuff_append(bbuff *bbuff, const void *src, size_t len)
{
    if(len == 0)
        return;
    
    if(bbuff->size < (bbuff->wpos + len))
    {
        bbuff_reserve(bbuff, bbuff->wpos + len);
        bbuff->size = bbuff->wpos + len;
    }
    
    memcpy(bbuff->storage + bbuff->wpos, src, len);
    bbuff->wpos += len;
}

void bbuff_read(bbuff *bbuff, void *dst, size_t len)
{
    if((bbuff->rpos + len) <= bbuff->size)
    {
        memcpy(dst, bbuff->storage + bbuff->rpos, len);
    }
    else
    {
        memset(dst, 0, len);
    }
    bbuff->rpos += len;
}

void bbuff_put(bbuff *bbuff, size_t pos, const void *src, size_t len)
{
    assert(pos + len <= bbuff->size);
    memcpy(bbuff->storage + pos, src, len);
}





