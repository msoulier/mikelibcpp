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
    // block until there is room on the queue.
    void enqueue(const T item);
    // Dequeue a new T and return it. If the queue is empty, wait on it
    // until it is not empty.
    T dequeue(void);
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
void SafeQueue<T>::enqueue(const T item) {

    std::unique_lock<std::mutex> lock(m_mutex);

    while ((m_maxsize != 0) && (m_queue.size() >= m_maxsize)) {
        // Queue full. Can't push more on. Block until there's room.
        // this will atomically unlock the mutex and wait for the cv to get
        // notified
        m_full.wait(lock);
    }
    m_queue.push(item);
    lock.unlock();
    m_empty.notify_all();
}

template<class T>
T SafeQueue<T>::dequeue(void) {

    std::unique_lock<std::mutex> lock(m_mutex);

    while (m_queue.size() == 0) {
        // Wait until something is put on it.
        // this will atomically unlock the mutex and wait for the cv 
        m_empty.wait(lock);
    }

    // Pull the item off and notify writer if it's waiting on full cond.
    T item = m_queue.front();
    m_queue.pop();
    lock.unlock();
    m_full.notify_all();
    return item;
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
