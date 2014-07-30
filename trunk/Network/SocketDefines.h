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


#ifndef SOCKET_DEFINES_H
#define SOCKET_DEFINES_H

/* Implementation Selection */
#include "../Defines.h"
#if defined(WIN32) && !defined(WP8)
    #define CONFIG_USE_IOCP
#elif WP8
    #define CONFIG_USE_WP8
#else
    #ifdef MAC
        //MAC sockets
        #define SOCKET 			int
        #define SD_BOTH 		SHUT_RDWR
        #define TCP_NODELAY 	0x6

        #include <sys/event.h>
        #define CONFIG_USE_KQUEUE
    #else
        // unix defines
        #define SOCKET 			int
        #define SD_BOTH 		SHUT_RDWR
        #define TCP_NODELAY 	0x6
	
        #include <sys/epoll.h>
        #define CONFIG_USE_EPOLL
    #endif
#endif

#define SOCKET_SEND_RECV_TIMEOUT 15
#define MAX_EVENTS 32

/* IOCP Defines */

#ifdef CONFIG_USE_IOCP
enum SocketIOEvent : size_t
{
	SOCKET_IO_EVENT_READ_COMPLETE   = 0,
	SOCKET_IO_EVENT_WRITE_END		= 1,
	SOCKET_IO_THREAD_SHUTDOWN		= 2,
	NUM_SOCKET_IO_EVENTS			= 3,
};

struct OverlappedStruct
{
	OVERLAPPED          m_overlap;
	size_t              m_event;
    std::atomic<bool>   m_inUse;

	explicit OverlappedStruct() : m_event(0), m_inUse(false)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
	}
    
	explicit OverlappedStruct(SocketIOEvent ev) : m_event(ev), m_inUse(false)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
	}

	void Reset(SocketIOEvent ev)
	{
		memset(&m_overlap, 0, sizeof(OVERLAPPED));
		m_event = static_cast<size_t>(ev);
	}

	void Mark()
	{
        if(m_inUse)
        {
            Log.Error(__FUNCTION__, "!!!! Network: Detected double use of read/write event! Previous event was %u.", (uint32)m_event);
        }

        m_inUse = true;
	}

	void Unmark()
	{
		m_inUse = false;
	}
};

#endif

#endif
