//
//  CDefines.h
//
//  Created by Miroslav Kudrnac on 10.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef CDEFINES_H
#define CDEFINES_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

//platform definitions
#ifdef __APPLE_CC__
    #define MAC
#endif
    
//Windows
#ifdef _M_X64
    #define X64
#endif
#ifdef _WIN32
    #define WIN32
#endif
#if !defined(DEBUG) && defined(WIN32)
//  #define _SECURE_SCL 0
//	#define _HAS_EXCEPTIONS 0
//	#define _CRT_DISABLE_PERFCRIT_LOCKS
#endif
#if defined(WIN32) && !defined(WP8)
    #define _CRT_SECURE_NO_WARNINGS
//	#define _SCL_SECURE_NO_WARNINGS
//	#define _HAS_ITERATOR_DEBUGGING 0
#endif
    
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

#ifdef WIN32
    #define localtime(a,b) localtime_s(b,a)
    #define snprintf _snprintf
    #define strnicmp _strnicmp
    #define stricmp _stricmp
    #define I64FMT "%016I64X"
    #define I64FMTD "%I64u"
    #define SI64FMTD "%I64d"
    #define atoll _atoi64
#else
    #define localtime localtime_r
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
    #define I64FMT "%016llX"
    #define I64FMTD "%llu"
    #define SI64FMTD "%lld"
#endif
    
#ifndef WIN32
    //imports
    #define MAX_PATH 				PATH_MAX
    #define INVALID_HANDLE_VALUE    -1
    
    static inline uint64 GetTickCount64()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }	
#endif
    
#ifdef __cplusplus
}
#endif
    
#endif
