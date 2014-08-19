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


#include "Network.h"

#ifdef CONFIG_USE_WP8

initialiseSingleton(SocketMgr);

SocketMgr::SocketMgr()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	FD_ZERO(&m_readableSet);
	FD_ZERO(&m_writableSet);
}

SocketMgr::~SocketMgr()
{
	WSACleanup();
}

void SocketMgr::SpawnWorkerThreads()
{
	ThreadPool.ExecuteTask(new SocketWorkerThread());
}

void SocketMgr::AddSocket(BaseSocket * pSocket, bool listenSocket)
{
	std::lock_guard<std::recursive_mutex> rGuard(m_socketLock);
	if (m_sockets.find(pSocket) == m_sockets.end())
	{
		if (pSocket->Writable())
			FD_SET(pSocket->GetFd(), &m_writableSet);
		else
			FD_SET(pSocket->GetFd(), &m_readableSet);
		m_sockets.insert(pSocket);
	}
}

void SocketMgr::RemoveSocket(BaseSocket * pSocket)
{
	std::lock_guard<std::recursive_mutex> rGuard(m_socketLock);
	if (m_sockets.find(pSocket) != m_sockets.end())
	{
		FD_CLR(pSocket->GetFd(), &m_readableSet);
		FD_CLR(pSocket->GetFd(), &m_writableSet);
		m_sockets.erase(pSocket);
	}
}

void SocketMgr::WantWrite(BaseSocket * pSocket)
{
	std::lock_guard<std::recursive_mutex> rGuard(m_socketLock);
	FD_SET(pSocket->GetFd(), &m_writableSet);
}

void SocketMgr::thread_func(ThreadContext *pContext)
{
	int fd_count;
	timeval rTimeout;
	rTimeout.tv_sec = 0;
	rTimeout.tv_usec = 0;
	FD_SET readable;
	FD_SET writable;
	BaseSocket *pSocket;
	SocketSet::iterator itr, itr2;

	for (;;)
	{
		m_socketLock.lock();
		if (m_sockets.empty())
		{
			m_socketLock.unlock();
			pContext->Wait(50);
			continue;
		}

		/* copy the sets */
		memcpy(&readable, &m_readableSet, sizeof(m_readableSet));
		memcpy(&writable, &m_writableSet, sizeof(m_writableSet));

		/** clear the writable set for the next loop 
			will work like EV_ONESHOT 
		*/
		FD_ZERO(&m_writableSet);

		//poll sockets status
		fd_count = select(FD_SETSIZE, &readable, &writable, NULL, &rTimeout);
		if (fd_count < 0)
		{
			Log.Error(__FUNCTION__, "select fd_count: %d errno: %d", fd_count, WSAGetLastError());
		}
		else if (fd_count > 0)
		{
			for (itr = m_sockets.begin(); itr != m_sockets.end();)
			{
				itr2 = itr;
				++itr;
				pSocket = (*itr2);

				// If the ReadSet is marked for this socket then this means data
				// is available to be read on the socket
				if (FD_ISSET(pSocket->GetFd(), &readable))
				{
					if (pSocket->IsConnected())
					{
						pSocket->ReadCallback(0);
					}
				}

				// If the WriteSet is marked on this socket then this means the internal
				// data buffers are available for more data
				if (FD_ISSET(pSocket->GetFd(), &writable))
				{
					if (pSocket->IsConnected())
					{
						pSocket->BurstBegin();                      // Lock receive mutex
						pSocket->WriteCallback(0);                  // Perform actual send()
						if (pSocket->Writable())
						{
							pSocket->PostEvent(EVFILT_WRITE);		// Still remaining data.
						}
						else
						{
							pSocket->DecSendLock();
						}
						pSocket->BurstEnd();						// Unlock
					}
				}
			}
		}
		//unlock socket set
		m_socketLock.unlock();

		//wait
		pContext->Wait(100);
	}
}

bool SocketWorkerThread::run()
{
	CommonFunctions::SetThreadName("SocketWorker thread");
	sSocketMgr.thread_func(this);
	return true;
}

void SocketMgr::CloseAll()
{
	LockingPtr<SocketSet> pSockets(m_sockets, NULL);
	std::list<BaseSocket*> tokill;

	m_socketLock.lock();
	for (SocketSet::iterator itr = pSockets->begin(); itr != pSockets->end(); ++itr)
	{
		tokill.push_back(*itr);
	}
	m_socketLock.unlock();
	
	for (std::list<BaseSocket*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
	{
		(*itr)->Disconnect();
	}

	size_t size = 0;
	do
	{
		m_socketLock.lock();
		size = pSockets->size();
		m_socketLock.unlock();
	}while(size);
}

#endif