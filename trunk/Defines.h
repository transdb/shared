/*
 * Game server
 * Copyright (C) 2010 Miroslav 'Wayland' Kudrnac
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DEFINES_H
#define DEFINES_H

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

//common headers - C/C++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

	#include <malloc.h>
	#include <time.h>
	#include <signal.h>
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <process.h>
#else
    #include <ctype.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <sys/times.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>

    #ifndef MAC
        #include <linux/types.h>
    #endif
#endif

//C++ stuff
#ifdef __cplusplus
    #include <set>
    #include <list>
    #include <map>
    #include <queue>
    #include <sstream>
#endif

//C++11 stuff - containers - shared_ptr
#ifdef __cplusplus
    #include <unordered_map>
    #include <unordered_set>
    #include <array>
    #include <memory>
    #include <mutex>
    #include <atomic>
    #include <condition_variable>
    #include <chrono>
    #include <thread>
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

    //imports
    #define MAX_PATH 				1024
    #define INVALID_HANDLE_VALUE    -1

    static inline uint64 GetTickCount64()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }
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
    #define NOEXCEPT _NOEXCEPT
#else
    #define localtime localtime_r
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
    #define I64FMT "%016llX"
    #define I64FMTD "%llu"
    #define SI64FMTD "%lld"
    #define NOEXCEPT noexcept
#endif

//time
extern time_t       UNIXTIME;
extern struct tm    g_localTime;

//inline
#ifdef DEBUG
    #define INLINE
#else
    #define INLINE inline
#endif

#ifdef WIN32
    #define NOINLINE __declspec(noinline)
#else
    #define NOINLINE __attribute__((noinline))
#endif

#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
    TypeName(const TypeName&);                  \
    void operator=(const TypeName&)

#endif