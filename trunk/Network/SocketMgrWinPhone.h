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

#ifndef SOCKETMGR_H_WIN32
#define SOCKETMGR_H_WIN32

#ifdef CONFIG_USE_WP8

class Socket;

typedef std::set<BaseSocket*> SocketSet;

#define EVFILT_WRITE	0
#define EVFILT_READ		1

class SocketMgr : public Singleton<SocketMgr>
{
public:
	SocketMgr();
	~SocketMgr();

	/// add a new socket to the epoll set and to the fd mapping
	void AddSocket(BaseSocket * pSocket, bool listenSocket);

	/// remove a socket from epoll set/fd mapping
	void RemoveSocket(BaseSocket * pSocket);

	/// closes all sockets
	void CloseAll();

	/// spawns worker threads
	void SpawnWorkerThreads(uint32 count);

	/// This is called when a socket has data to write for the first time.
	void WantWrite(BaseSocket * pSocket);

	/// Thread proc
	void thread_func(ThreadContext *pContext);

private:
	FD_SET					m_allSet;
	FD_SET					m_writableSet;
	FD_SET					m_readableSet;
	FD_SET					m_exceptionSet;
	uint32					m_socket_count;

	SocketSet				m_sockets;
	Mutex					m_socketLock;
	volatile long			m_threadcount;
};

#define sSocketMgr SocketMgr::getSingleton()

class SocketWorkerThread : public ThreadContext
{
public:
	bool run();
};

#endif
#endif