//
//  CVector.c
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#include "CVector.h"

CVector *CVector_create(size_t itemSize)
{
    CVector *pCVector = calloc(1, sizeof(CVector));
    if(pCVector == NULL)
        return NULL;
    
    pCVector->m_itemSize = itemSize;
    return pCVector;
}

void CVector_destroy(CVector *pCVector)
{
    free(pCVector->m_pBuff);
    free(pCVector);
}

void CVector_clear(CVector *pCVector)
{
    free(pCVector->m_pBuff);
    pCVector->m_size = 0;
    pCVector->m_capacity = 0;
}

void CVector_reserve(CVector *pCVector, size_t newCapacity)
{
    assert(newCapacity >= 0);
    
    if(newCapacity > pCVector->m_capacity)
    {
        //set capacity
        pCVector->m_capacity = newCapacity;
        //resize buffer
        void *pNewBuffer = realloc(pCVector->m_pBuff, pCVector->m_itemSize * newCapacity);
        if(pNewBuffer == NULL)
            return;
        
        pCVector->m_pBuff = pNewBuffer;
    }
}

void CVector_resize(CVector *pCVector, size_t newSize)
{
    CVector_reserve(pCVector, newSize);
    pCVector->m_size = newSize;
}

void CVector_push_back(CVector *pCVector, const void *pValue)
{
    if(pCVector->m_capacity == pCVector->m_size)
    {
        size_t newCapacity = pCVector->m_capacity == 0 ? 16 : pCVector->m_capacity * 2;
        CVector_reserve(pCVector, newCapacity);
    }
    
    void *pDest = (pCVector->m_pBuff + (pCVector->m_itemSize * pCVector->m_size));
    memcpy(pDest, pValue, pCVector->m_itemSize);
    ++pCVector->m_size;
}










