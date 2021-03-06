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

#ifdef CONFIG_USE_IOCP

initialiseSingleton(SocketMgr);

SocketMgr::SocketMgr()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);
}

SocketMgr::~SocketMgr()
{
	CloseHandle(m_completionPort);
}

void SocketMgr::SpawnWorkerThreads()
{
    ThreadPool.ExecuteTask(new SocketWorkerThread());
}

bool SocketWorkerThread::run()
{
    CommonFunctions::SetThreadName("SocketWorker thread");
    //
	HANDLE cp = sSocketMgr.GetCompletionPort();
	DWORD len;
	Socket * s;
	OverlappedStruct * ov;
	LPOVERLAPPED ol_ptr;

	for(;;)
	{
		if(!GetQueuedCompletionStatus(cp, &len, (PULONG_PTR)&s, &ol_ptr, INFINITE))
			continue;

		ov = CONTAINING_RECORD(ol_ptr, OverlappedStruct, m_overlap);

		if(ov->m_event == SOCKET_IO_THREAD_SHUTDOWN)
		{
			delete ov;
			return true;
		}

		if(ov->m_event < NUM_SOCKET_IO_EVENTS)
		{
			g_ophandlers[ov->m_event](s, static_cast<size_t>(len));
		}
	}

	return true;
}

void HandleReadComplete(Socket * s, size_t len)
{
	if(!s->IsDeleted())
	{
		s->GetReadEvent().Unmark();
		if(len)
		{
            s->ReadCallback(len);
		}
		else
			s->Delete();	  // Queue deletion.
	}
}

void HandleWriteComplete(Socket * s, size_t len)
{
	if(!s->IsDeleted())
	{
		s->GetWriteEvent().Unmark();
		s->BurstBegin();					// Lock
		s->GetWriteBuffer().Remove(len);
		if(s->GetWriteBuffer().GetContiguiousBytes() > 0)
			s->WriteCallback(len);
		else
			s->DecSendLock();
		s->BurstEnd();						// Unlock
	}
}

void SocketMgr::CloseAll()
{
	LockingPtr<SocketSet> pSockets(m_sockets, NULL);
	std::list<Socket*> tokill;

	m_socketLock.lock();
	for (SocketSet::iterator itr = pSockets->begin(); itr != pSockets->end(); ++itr)
	{
		tokill.push_back(*itr);
	}
	m_socketLock.unlock();
	
	for (std::list<Socket*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
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

void SocketMgr::ShutdownThreads()
{
    OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_THREAD_SHUTDOWN);
    PostQueuedCompletionStatus(m_completionPort, 0, (ULONG_PTR)0, &ov->m_overlap);
}

#endif