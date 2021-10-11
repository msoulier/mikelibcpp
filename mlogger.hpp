//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#ifndef mlogger_hpp
#define mlogger_hpp

#include <stdio.h>
#include <ostream>
#include <string>
#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#define LOGLEVEL_TRACE 0
#define LOGLEVEL_DEBUG 10
#define LOGLEVEL_INFO 20
#define LOGLEVEL_WARN 30
#define LOGLEVEL_ERROR 40

//////////////////////////////////////////////////////////////////////
// SimpleLogger - A first pass at a thread-safe logger.             //
//////////////////////////////////////////////////////////////////////

class SimpleLogger
{
public:
    SimpleLogger();
    ~SimpleLogger();
    // Set the current logging level
    void setLevel(int level);
    // Get the current logging level
    int getLevel();
    // Convenience methods for trace level log.
    void trace(const char *msg);
    void trace(std::string msg);
    // Convenience methods for debug level log
    void debug(const char *msg);
    void debug(std::string msg);
    // Convenience methods for info level log
    void info(const char *msg);
    void info(std::string msg);
    // Convenience methods for warning level log
    void warn(const char *msg);
    void warn(std::string msg);
    // Convenicence methods for error level log
    void error(const char *msg);
    void error(std::string msg);
private:
    // The current log level.
    int m_level;
    // The mutex used for synchronization.
    boost::mutex m_mutex;
    // The output stream for the logger. FIXME: implement logging handlers
    std::ostream& m_ostream;
    // The actual main logging function.
    void log(int level, std::string msg);
    // Return the current date and time as a localized string.
    const std::string localDateTime();
};

//////////////////////////////////////////////////////////////////////
// MLogger - A second pass at a C++, thread-safe logger.            //
//     Note that it returns MLoggerHandler objects to do the actual //
//     logging.                                                     //
//////////////////////////////////////////////////////////////////////

extern boost::thread_specific_ptr<std::stringstream> tls_buffer;

class MLoggerHandler
{
public:
    MLoggerHandler(boost::mutex& mutex,
                   std::ostream& ostream,
                   int threshold,
                   std::string prefix);
    ~MLoggerHandler();
    void setLevel(int level);

    template <class T>
    // For handling << from any object.
    MLoggerHandler& operator <<(T input) {
        // Only log if the level is set above our threshold.
        if (m_threshold >= m_level) {
            if (tls_buffer.get() == NULL) {
                tls_buffer.reset(new std::stringstream());
            }
            if (tls_buffer->str().length() == 0) {
                *tls_buffer << localDateTime() << " " << m_prefix << ": " << input;
            }
            else {
                *tls_buffer << input;
            }
        }
        return *this;
    }
    // For handling std::endl
    std::ostream& operator <<(std::ostream& (*f)(std::ostream&)) {
        // Only log if the level is set above our threshold.
        if (m_threshold >= m_level) {
            boost::lock_guard<boost::mutex> lock{m_mutex};
            // Flush the buffer
            m_ostream << tls_buffer->str();
            f(m_ostream);
            // Clear the buffer
            tls_buffer->str("");
        }
        return m_ostream;
    }
private:
    // A mutex passed in from the main logger for synchronization.
    boost::mutex& m_mutex;
    // The logging level.
    int m_level;
    // The output stream.
    std::ostream& m_ostream;
    // The threshold for logging for this handler.
    int m_threshold;
    // The string prefix for logging.
    std::string m_prefix;
    // Return the current date and time as a localized string.
    const std::string localDateTime();
};

/*
 * The MLogger (Mike-logger) is a thread-safe C++ logger using the iostream operators.
 * To use it, you must invoke a logging level handler which will return an
 * MLoggerHandler reference, and then terminate your line with std::endl to ensure
 * that the buffer is flushed and the line terminated with a newline.
 */
class MLogger
{
public:
    MLogger();
    MLogger(std::string name);
    ~MLogger();
    // Set the current logging level
    void setLevel(int level);
    // Get the current logging level
    int getLevel();
    // Convenience methods for trace level log.
    MLoggerHandler& trace();
    // Convenience methods for debug level log
    MLoggerHandler& debug();
    // Convenience methods for info level log
    MLoggerHandler& info();
    // Convenience methods for warning level log
    MLoggerHandler& warn();
    // Convenicence methods for error level log
    MLoggerHandler& error();
private:
    // The logger name.
    std::string m_name;
    // The current log level.
    int m_level;
    // The output stream for the logger.
    std::ostream& m_ostream;
    // The mutex used for synchronization.
    boost::mutex m_mutex;
    // Trace handler
    MLoggerHandler m_trace_handler;
    // Debug handler
    MLoggerHandler m_debug_handler;
    // Info handler
    MLoggerHandler m_info_handler;
    // Warn handler
    MLoggerHandler m_warn_handler;
    // Error handler
    MLoggerHandler m_error_handler;
};

extern MLogger logger;

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
    boost::mutex m_mutex;
    boost::condition m_empty;
    boost::condition m_full;
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
    boost::lock_guard<boost::mutex> lock{m_mutex};
    if ((m_maxsize != 0) && (m_queue.size() == m_maxsize)) {
        // Queue full. Can't push more on. Block until there's room.
        m_full.wait(m_mutex);
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
    boost::lock_guard<boost::mutex> lock{m_mutex};
    if (m_queue.empty()) {
        // Wait until something is put on it.
        m_empty.wait(m_mutex);
    }
    // Pull the item off and notify writer if it's waiting on full cond.
    item = m_queue.front();
    m_queue.pop();
    m_full.notify_all();
}


#endif /* mlogger_hpp */
