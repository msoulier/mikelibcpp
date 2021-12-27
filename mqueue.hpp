//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#ifndef mqueue_hpp
#define mqueue_hpp

#include <queue>
#include <mutex>
#include <condition_variable>

//////////////////////////////////////////////////////////////////////
// SafeQueue - A thread-safe templated queue.                       //
//////////////////////////////////////////////////////////////////////
template<class T>
class SafeQueue
{
public:
    // Instantiate a new queue. 0 maxsize means unlimited.
    SafeQueue(size_t maxsize = 0);
    ~SafeQueue(void);
    // Enqueue a new T. If enqueue would cause it to exceed maxsize,
    // and block is true, block until there is room on the queue. Return
    // true on a successful enqueue, false otherwise.
    bool enqueue(const T item, bool block=false);
    // Dequeue a new T, populating the item referenced passed. If the queue
    // is empty and block is true, block until the queue is not empty.
    // Returns true on a successful dequeue, false otherwise.
    bool dequeue(T& item, bool block=false);
    // Return size of the queue.
    size_t size(void);
    // Return the maxsize of the queue.
    size_t maxsize(void) const;
private:
    std::mutex m_mutex;
    std::condition_variable m_empty;
    std::condition_variable m_full;
    std::queue<T> m_queue;
    size_t m_maxsize;
};

template<class T>
SafeQueue<T>::SafeQueue(size_t maxsize) : m_maxsize(maxsize) { }

template<class T>
SafeQueue<T>::~SafeQueue(void) { }

template<class T>
bool SafeQueue<T>::enqueue(const T item, bool block) {

    std::unique_lock<std::mutex> lock(m_mutex);

    if (block) {
        while ((m_maxsize != 0) && (m_queue.size() >= m_maxsize)) {
            // Queue full. Can't push more on. Block until there's room.
            // this will atomically unlock the mutex and wait for the cv to get
            // notified
            m_full.wait(lock);
        }
    } else {
        if ((m_maxsize != 0) && (m_queue.size() >= m_maxsize)) {
            // Queue full. Can't push more on. Do not block.
            return false;
        }
    }
    m_queue.push(item);
    lock.unlock();
    m_empty.notify_all();
    return true;
}

template<class T>
bool SafeQueue<T>::dequeue(T& item, bool block) {

    std::unique_lock<std::mutex> lock(m_mutex);

    if (block) {
        while (m_queue.size() == 0) {
            // Wait until something is put on it.
            // this will atomically unlock the mutex and wait for the cv 
            m_empty.wait(lock);
        }
    } else {
        if (m_queue.size() == 0) {
            // Nothing to dequeue and we don't want to block.
            return false;
        }
    }

    // Pull the item off and notify writer if it's waiting on full cond.
    item = m_queue.front();
    m_queue.pop();
    lock.unlock();
    m_full.notify_all();
    return true;
}

template<class T>
size_t SafeQueue<T>::size(void) {

    std::unique_lock<std::mutex> lock(m_mutex);
    return m_queue.size();
}

template<class T>
size_t SafeQueue<T>::maxsize(void) const {
    return m_maxsize;
}

#endif /* mqueue_hpp */
