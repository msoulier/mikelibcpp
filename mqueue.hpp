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
    // Enqueue a new T. If enqueue would cause it to exceed maxsize,
    // block until there is room on the queue.
    void enqueue(const T& item);
    // Dequeue a new T and return it. If the queue is empty, wait on it
    // until it is not empty.
    T& dequeue(void);
    // Return size of the queue.
    size_t size(void) const;
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
SafeQueue<T>::SafeQueue(unsigned int maxsize) : m_maxsize(maxsize) { }

template<class T>
SafeQueue<T>::~SafeQueue() { }

template<class T>
void SafeQueue<T>::enqueue(const T& item) {
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
T& SafeQueue<T>::dequeue(void) {
    // Synchronize. No unlock needed due to unique lock.
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
        // Wait until something is put on it.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_empty.wait(lock);
    }
    // Pull the item off and notify writer if it's waiting on full cond.
    T& item = m_queue.front();
    m_queue.pop();
    m_full.notify_all();
    return item;
}

template<class T>
size_t SafeQueue<T>::size(void) const {
    return m_queue.size();
}

template<class T>
size_t SafeQueue<T>::maxsize(void) const {
    return m_maxsize;
}

#endif /* mqueue_hpp */
