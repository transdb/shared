//
//  ConcurrentQueue.h
//
//  Created by Miroslav Kudrnac on 29.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef TransDB_ConcurrentQueue_h
#define TransDB_ConcurrentQueue_h

template <class T>
class ConcurrentQueue
{
public:
    bool push(const T &rItem)
    {
        std::lock_guard<std::mutex> rLock(m_rLock);
        m_rQueue.push(rItem);
        m_rCond.notify_one();
        return true;
    }
    
    bool timeout_pop(T &rItem, uint64 timeout)
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        if(m_rQueue.empty())
        {
            if(m_rCond.wait_for(rLock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout)
                return false;
        }
        
        rItem = m_rQueue.front();
        m_rQueue.pop();
        return true;
    }
    
private:
    std::queue<T>               m_rQueue;
    std::condition_variable     m_rCond;
    std::mutex                  m_rLock;
};

#endif
