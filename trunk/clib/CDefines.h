//
//  CDefines.h
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef CDEFINES_H
#define CDEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

//data types
#ifdef WIN32
    typedef signed __int64 int64;
    typedef signed __int32 int32;
    typedef signed __int16 int16;
    typedef signed __int8 int8;

    typedef unsigned __int64 uint64;
    typedef unsigned __int32 uint32;
    typedef unsigned __int16 uint16;
    typedef unsigned __int8 uint8;
#else
    typedef int64_t int64;
    typedef int32_t int32;
    typedef int16_t int16;
    typedef int8_t int8;

    typedef uint64_t uint64;
    typedef uint32_t uint32;
    typedef uint16_t uint16;
    typedef uint8_t uint8;

    //MS datatypes
    typedef uint32_t    DWORD;
    typedef intptr_t    INT_PTR;
    typedef size_t      SIZE_T;
    typedef uint32_t    UINT;
    typedef uint8_t     BYTE;
    typedef int32_t     INT;
    typedef int         HANDLE;
#endif

#ifdef __cplusplus
}
#endif
    
#endif
