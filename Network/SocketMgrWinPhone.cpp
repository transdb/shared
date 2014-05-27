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

	FD_ZERO(&m_allSet);
	FD_ZERO(&m_readableSet);
	FD_ZERO(&m_writableSet);
	FD_ZERO(&m_exceptionSet);
	m_socket_count = 0;
}

SocketMgr::~SocketMgr()
{

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
		FD_SET(pSocket->GetFd(), &m_allSet);
		m_sockets.insert(pSocket);
		++m_socket_count;
	}
}

void SocketMgr::RemoveSocket(BaseSocket * pSocket)
{
	std::lock_guard<std::recursive_mutex> rGuard(m_socketLock);
	if (m_sockets.find(pSocket) != m_sockets.end())
	{
		FD_CLR(pSocket->GetFd(), &m_allSet);
		m_sockets.erase(pSocket);
		--m_socket_count;
	}
}

void SocketMgr::WantWrite(BaseSocket * pSocket)
{
	std::lock_guard<std::recursive_mutex> rGuard(m_socketLock);
	FD_SET(pSocket->GetFd(), &m_writableSet);
}

void SocketMgr::thread_func(ThreadContext *pContext)
{
	size_t r_size = sizeof(m_allSet);
	int fd_count;
	timeval rTimeout;
	rTimeout.tv_sec = 0;
	rTimeout.tv_usec = 0;
	FD_SET writable;
	BaseSocket *pSocket;
	SocketSet::iterator itr, itr2;

	for (;;)
	{
		m_socketLock.lock();
		if (m_socket_count == 0)
		{
			m_socketLock.unlock();
			pContext->Wait(50);
			continue;
		}

		/* copy the all set into the readable set */
		memcpy(&m_readableSet, &m_allSet, r_size);
		memcpy(&writable, &m_writableSet, r_size);

		/* clear the writable set for the next loop */
		FD_ZERO(&m_writableSet);

		m_socketLock.lock();
		fd_count = select(FD_SETSIZE, &m_readableSet, &writable, &m_exceptionSet, &rTimeout);
		m_socketLock.unlock();
		if (fd_count < 0)
		{
			Log.Error(__FUNCTION__, "select fd_count: %d errno: %d", fd_count, WSAGetLastError());
		}
		else if (fd_count > 0)
		{
			LockingPtr<SocketSet> pSockets(m_sockets, NULL);
			if (pSockets->size())
			{
				for (itr = pSockets->begin(); itr != pSockets->end();)
				{
					itr2 = itr;
					++itr;
					pSocket = (*itr2);

					if (FD_ISSET(pSocket->GetFd(), &m_readableSet))
					{
						pSocket->ReadCallback(0);
						if (pSocket->Writable() && pSocket->IsConnected())
						{
							pSocket->BurstPush();
						}
					}
					else if (FD_ISSET(pSocket->GetFd(), &writable))
					{
						pSocket->BurstBegin();                          // Lock receive mutex
						pSocket->WriteCallback(0);                      // Perform actual send()
						if (pSocket->Writable() && pSocket->IsConnected())
						{
							pSocket->PostEvent(EVFILT_WRITE);     // Still remaining data.
						}
						else
						{
							pSocket->DecSendLock();
						}
						pSocket->BurstEnd();                            // Unlock
					}
					else if (FD_ISSET(pSocket->GetFd(), &m_exceptionSet))
					{
						pSocket->Disconnect();
					}
				}
			}
		}

		/* clear the exception set for the next loop */
		FD_ZERO(&m_exceptionSet);
		m_socketLock.unlock();
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