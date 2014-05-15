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

void SocketMgr::SpawnWorkerThreads(uint32 count)
{
	ThreadPool.ExecuteTask(new SocketWorkerThread());
}

void SocketMgr::AddSocket(BaseSocket * pSocket, bool listenSocket)
{
	LockingPtr<SocketSet> pSockets(m_sockets, m_socketLock);
	if (pSockets->find(pSocket) == pSockets->end())
	{
		FD_SET(pSocket->GetFd(), &m_allSet);
		pSockets->insert(pSocket);
		++m_socket_count;
	}
}

void SocketMgr::RemoveSocket(BaseSocket * pSocket)
{
	LockingPtr<SocketSet> pSockets(m_sockets, m_socketLock);
	if (pSockets->find(pSocket) != pSockets->end())
	{
		FD_CLR(pSocket->GetFd(), &m_allSet);
		pSockets->erase(pSocket);
		--m_socket_count;
	}
}

void SocketMgr::WantWrite(BaseSocket * pSocket)
{
	m_socketLock.Acquire();
	FD_SET(pSocket->GetFd(), &m_writableSet);
	m_socketLock.Release();
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
		m_socketLock.Acquire();
		if (m_socket_count == 0)
		{
			m_socketLock.Release();
			pContext->Wait(50);
			continue;
		}

		/* copy the all set into the readable set */
		memcpy(&m_readableSet, &m_allSet, r_size);
		memcpy(&writable, &m_writableSet, r_size);

		/* clear the writable set for the next loop */
		FD_ZERO(&m_writableSet);

		m_socketLock.Acquire();
		fd_count = select(FD_SETSIZE, &m_readableSet, &writable, &m_exceptionSet, &rTimeout);
		m_socketLock.Release();
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
		m_socketLock.Release();
	}
}

bool SocketWorkerThread::run()
{
	SetThreadName("SocketWorker thread");
	sSocketMgr.thread_func(this);
	return true;
}

void SocketMgr::CloseAll()
{
	list<BaseSocket*> tokill;

	m_socketLock.Acquire();
	for(SocketSet::iterator itr = m_sockets.begin(); itr != m_sockets.end(); ++itr)
	{
		tokill.push_back(*itr);
	}
	m_socketLock.Release();
	
	for (list<BaseSocket*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
	{
		(*itr)->Disconnect();
	}

	size_t size = 0;
	do
	{
		m_socketLock.Acquire();
		size = m_sockets.size();
		m_socketLock.Release();
	}while(size);
}

#endif