//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#ifndef mqueue_hpp
#define mqueue_hpp

#include <queue>
#include <mutex>

//////////////////////////////////////////////////////////////////////
// SafeQueue - A thread-safe templated queue.                       //
//////////////////////////////////////////////////////////////////////
template<class T>
class SafeQueue
{
public:
    // Instantiate a new queue. 0 maxsize means unlimited.
    SafeQueue(unsigned int maxsize = 0);
    ~SafeQueue(void);
    // Enqueue a new T. Returns true on success, false if the queue is full.
    void enqueue(T& item);
    // Dequeue a new T. Returns true if something was dequeued, false if the
    // queue was empty.
    void dequeue(T& item);
private:
    std::mutex m_mutex;
    std::condition_variable m_empty;
    std::condition_variable m_full;
    std::queue<T> m_queue;
    unsigned int m_maxsize;
};

template<class T>
SafeQueue<T>::SafeQueue(unsigned int maxsize) : m_maxsize(maxsize) { }

template<class T>
SafeQueue<T>::~SafeQueue() { }

template<class T>
void SafeQueue<T>::enqueue(T& item) {
    // Synchronize. No unlock needed due to unique_lock.
    std::lock_guard<std::mutex> lock(m_mutex);
    if ((m_maxsize != 0) && (m_queue.size() == m_maxsize)) {
        // Queue full. Can't push more on. Block until there's room.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_full.wait(lock);
    }
    else {
        // Add to m_queue and notify the reader if it's waiting.
        m_queue.push(item);
        m_empty.notify_all();
    }
}

template<class T>
void SafeQueue<T>::dequeue(T& item) {
    // Synchronize. No unlock needed due to unique lock.
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        // Wait until something is put on it.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_empty.wait(lock);
    }
    // Pull the item off and notify writer if it's waiting on full cond.
    item = m_queue.front();
    m_queue.pop();
    m_full.notify_all();
}

#endif /* mqueue_hpp */
