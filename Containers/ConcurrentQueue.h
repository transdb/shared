//
//  ConcurrentQueue.h
//
//  Created by Miroslav Kudrnac on 29.09.14.
//  Copyright (c) 2014 Miroslav Kudrnac. All rights reserved.
//

#ifndef TransDB_ConcurrentQueue_h
#define TransDB_ConcurrentQueue_h

//typedef struct ConcurrentQueue
//{
//    std::queue<T>               queue;
//    std::condition_variable     not_empty;
//    std::condition_variable     not_full;
//    std::condition_variable     all_tasks_done;
//    std::mutex                  mutex;
//    size_t                      maxsize;
//    size_t                      unfinished_tasks;
//} con_queue;

template <class T>
class ConcurrentQueue
{
public:
    explicit ConcurrentQueue(size_t maxsize = 0) NOEXCEPT : m_maxsize(maxsize)
    {
    }
    
    /** Return the approximate size of the queue (not reliable!).
     */
    size_t qsize()
    {
        std::lock_guard<std::mutex> rLock(m_rLock);
        return m_rQueue.size();
    }
    
    /** Put an item into the queue.
     *  If optional args 'block' is true and 'timeout' is 0 (the default),
     *  block if necessary until a free slot is available. If 'timeout' is
     *  a non-negative number, it blocks at most 'timeout' seconds and returns
     *  false if no free slot was available within that time.
     *  Otherwise ('block' is false), put an item on the queue if a free slot
     *  is immediately available, else return false ('timeout'
     *  is ignored in that case).
     */
    bool put(const T &item, bool block = true, uint32 timeout = 0)
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        if(m_maxsize > 0)
        {
            if(!block)
            {
                if(m_rQueue.size() == m_maxsize)
                    return false;
            }
            else if(timeout == 0)
            {
                while(m_rQueue.size() == m_maxsize)
                {
                    m_rNotFullCond.wait(rLock);
                }
            }
            else
            {
#if 0
                time_t endtime = UNIXTIME + timeout;
                while(m_rQueue.empty())
                {
                    time_t remaining = endtime - UNIXTIME;
                    if(remaining <= 0)
                        return false;
                    
                    not_full.wait_for(rLock, std::chrono::seconds(remaining));
                }
#else
                bool status = m_rNotFullCond.wait_for(rLock, std::chrono::seconds(timeout), [this](){ return !m_rQueue.empty(); } );
                if(status == false)
                    return false;
#endif
            }
        }
        
        m_rQueue.push(item);
        m_rNotEmptyCond.notify_one();
        return true;
    }
    
    /** Remove and return an item from the queue.
     *
     * If optional args 'block' is true and 'timeout' is 0 (the default),
     * block if necessary until an item is available. If 'timeout' is
     * a non-negative number, it blocks at most 'timeout' seconds and returns
     * false if no item was available within that time.
     * Otherwise ('block' is false), return an item if one is immediately
     * available, else return false ('timeout' is ignored
     * in that case).
     */
    bool get(T &item, bool block = true, uint32 timeout = 0)
    {
        std::unique_lock<std::mutex> rLock(m_rLock);
        if(!block)
        {
            if(m_rQueue.empty())
                return false;
        }
        else if(timeout == 0)
        {
            while(m_rQueue.empty())
            {
                m_rNotEmptyCond.wait(rLock);
            }
        }
        else
        {
#if 0
            time_t endtime = UNIXTIME + timeout;
            while(m_rQueue.empty())
            {
                time_t remaining = endtime - UNIXTIME;
                if(remaining <= 0)
                    return false;
                
                not_empty.wait_for(rLock, std::chrono::seconds(remaining));
            }
#else
            bool status = m_rNotEmptyCond.wait_for(rLock, std::chrono::seconds(timeout), [this](){ return !m_rQueue.empty(); } );
            if(status == false)
                return false;
#endif
        }
        
        item = m_rQueue.front();
        m_rQueue.pop();
        m_rNotFullCond.notify_one();
        return true;
    }
    
    /** unblock all waiting threads
     */
    INLINE void abort()
    {
        std::lock_guard<std::mutex> rLock(m_rLock);
        m_rNotEmptyCond.notify_all();
        m_rNotFullCond.notify_all();
    }
    
private:
    DISALLOW_COPY_AND_ASSIGN(ConcurrentQueue);
    
    //variables
    std::queue<T>               m_rQueue;
    std::condition_variable     m_rNotEmptyCond;
    std::condition_variable     m_rNotFullCond;
    std::mutex                  m_rLock;
    size_t                      m_maxsize;
};

#endif
