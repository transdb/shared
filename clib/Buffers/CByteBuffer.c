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

void bbuff_destroy(bbuff *self)
{
    free(self->storage);
    free(self);
}

void bbuff_reserve(bbuff *self, size_t ressize)
{
    assert(ressize >= 0);
    
    if(ressize > self->capacity)
    {
        //set capacity
        self->capacity = ressize;
        //resize buffer
        void *pNewBuffer = realloc(self->storage, ressize);
        if(pNewBuffer == NULL)
            return;
        
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
    if(len == 0)
        return;
    
    if(self->size < (self->wpos + len))
    {
        bbuff_reserve(self, self->wpos + len);
        self->size = self->wpos + len;
    }
    
    memcpy(self->storage + self->wpos, src, len);
    self->wpos += len;
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





