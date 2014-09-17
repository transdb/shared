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

//data types - platform
#include "clib/CDefines.h"

//system includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <fstream>
#include <assert.h>
#include <bitset>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <windows.h>

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <process.h>
#else
    #include <sys/mman.h>
	#include <sys/time.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
    #include <sys/times.h>
    #include <sys/resource.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <netdb.h>
    #include <fcntl.h>
#ifndef MAC
	#include <linux/types.h>
#endif
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <climits>
#include <signal.h>

//C++11 stuff - containers - shared_ptr
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <memory>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <thread>

#ifndef WIN32
	#include <pthread.h>
#endif

//time
extern time_t   UNIXTIME;	
extern tm       g_localTime;

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

//singleton
#include "Singleton.h"

//common functions
#include "CommonFunctions.h"

#endif