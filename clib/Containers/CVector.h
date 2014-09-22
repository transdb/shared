//
//  CVector.h
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef CVECTOR_H
#define CVECTOR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Defines.h"

typedef struct
{
    uint8   *m_pBuff;
    size_t  m_size;
    size_t  m_capacity;
    size_t  m_itemSize;
} CVector;


/**
 */
CVector *CVector_create(size_t itemSize);

/**
 */
void CVector_destroy(CVector *pCVector);

/**
 */
void CVector_clear(CVector *pCVector);

/**
 */
void CVector_reserve(CVector *pCVector, size_t newCapacity);

/**
 */
void CVector_resize(CVector *pCVector, size_t newSize);

/**
 */
void CVector_push_back(CVector *pCVector, const void *pValue);


#ifdef __cplusplus
}
#endif

#endif
