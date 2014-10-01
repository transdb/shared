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
    explicit ConcurrentQueue() NOEXCEPT : m_maxsize(0)
    {
    }
    
    /** Set max queue capacity
     *  is not thread safe should be called only on startup
     */
    INLINE void set_capacity(size_t maxsize) NOEXCEPT
    {
        m_maxsize = maxsize;
    }
    
    /** Put item into queue if full wait for space
     *  timeout is in seconds
     */
    bool put(const T &item, bool block = true, time_t timeout = 0)
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        if(m_maxsize > 0)
        {
            if(!block)
            {
                if(queue.size() == m_maxsize)
                    return false;
            }
            else if(timeout == 0)
            {
                while(queue.size() == m_maxsize)
                {
                    m_rNotFullCond.wait(rLock);
                }
            }
            else
            {
//                time_t endtime = UNIXTIME + timeout;
//                while(queue.empty())
//                {
//                    time_t remaining = endtime - UNIXTIME;
//                    if(remaining <= 0)
//                        return false;
//                    
//                    not_full.wait_for(rLock, std::chrono::seconds(remaining));
//                }
                
                bool status = m_rNotFullCond.wait_for(rLock, std::chrono::seconds(timeout), [this](){ return !queue.empty(); } );
                if(status == false)
                    return false;
            }
        }
        
        queue.push(item);
        m_rNotEmptyCond.notify_one();
        return true;
    }
    
    /** get item from queue
     *  wait for item
     *  timeout is in seconds
     */
    bool get(T &item, bool block = true, time_t timeout = 0)
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        if(!block)
        {
            if(queue.empty())
                return false;
        }
        else if(timeout == 0)
        {
            while(queue.empty())
            {
                m_rNotEmptyCond.wait(rLock);
            }
        }
        else
        {
//            time_t endtime = UNIXTIME + timeout;
//            while(queue.empty())
//            {
//                time_t remaining = endtime - UNIXTIME;
//                if(remaining <= 0)
//                    return false;
//                
//                not_empty.wait_for(rLock, std::chrono::seconds(remaining));
//            }
            
            bool status = m_rNotEmptyCond.wait_for(rLock, std::chrono::seconds(timeout), [this](){ return !queue.empty(); } );
            if(status == false)
                return false;
        }
        
        item = queue.front();
        queue.pop();
        m_rNotFullCond.notify_one();
        return true;
    }
    
    /**
     */
    INLINE size_t size() const NOEXCEPT
    {
        return 0;
    }
    
    /** unblock all waiting threads
     */
    INLINE void abort()
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        m_rNotEmptyCond.notify_all();
        m_rNotFullCond.notify_all();
    }
    
private:
    DISALLOW_COPY_AND_ASSIGN(ConcurrentQueue);
    
    //variable
    std::queue<T>               queue;
    std::condition_variable     m_rNotEmptyCond;
    std::condition_variable     m_rNotFullCond;
    std::mutex                  m_rLock;
    size_t                      m_maxsize;
};

#endif
