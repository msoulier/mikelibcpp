//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#ifndef mlogger_hpp
#define mlogger_hpp

#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <vector>

#define LOGLEVEL_TRACE 0
#define LOGLEVEL_DEBUG 10
#define LOGLEVEL_INFO 20
#define LOGLEVEL_WARN 30
#define LOGLEVEL_ERROR 40

//////////////////////////////////////////////////////////////////////
// MLogger - A second pass at a C++, thread-safe logger.            //
//     Note that it returns MLoggerEmitter objects to do the actual //
//     logging.                                                     //
//////////////////////////////////////////////////////////////////////

/*
 * This is the base class for all logging handlers.
 */
class MLoggerHandler
{
public:
    MLoggerHandler();
    virtual ~MLoggerHandler() = default;
    void operator<< (std::string buffer);
private:
    virtual void handle(std::string buffer);
};

/*
 * A stderr logger implementation.
 */
class MLoggerStderrHandler: public MLoggerHandler
{
public:
    MLoggerStderrHandler();
    ~MLoggerStderrHandler();
private:
    void handle(std::string buffer);
};

class MLoggerEmitter
{
public:
    MLoggerEmitter(std::stringstream& buffer,
                   std::mutex& mutex,
                   std::ostream& ostream,
                   int threshold,
                   std::string prefix,
                   std::vector<MLoggerHandler*> &handlers);
    ~MLoggerEmitter();
    void setLevel(int level);

    template <class T>
    // For handling << from any object.
    MLoggerEmitter& operator <<(T input) {
        // Only log if the level is set above our threshold.
        if (m_threshold >= m_level) {
            std::lock_guard<std::mutex> lock(m_mutex);
            // FIXME: not sure what this reset was for
            //if (m_buffer.get() == NULL) {
            //    m_buffer.reset(new std::stringstream());
            //}
            if (m_buffer.str().length() == 0) {
                m_buffer << localDateTime() << " " << m_prefix << ": " << input;
            }
            else {
                m_buffer << input;
            }
        }
        return *this;
    }
    // For handling std::endl
    std::ostream& operator <<(std::ostream& (*f)(std::ostream&)) {
        // Only log if the level is set above our threshold.
        if (m_threshold >= m_level) {
            std::lock_guard<std::mutex> lock(m_mutex);
            // Loop on the handlers and send the buffer to each.
            for (auto handler : m_handlers) {
                *handler << m_buffer.str();
            }

            // Flush the buffer
            //m_ostream << m_buffer.str();
            //f(m_ostream);

            //MLoggerStderrHandler handler;
            //handler << m_buffer.str();

            // Clear the buffer
            m_buffer.str("");
        }
        return m_ostream;
    }
private:
    // The thread-safe buffer where the logs are composed.
    std::stringstream& m_buffer;
    // A mutex passed in from the main logger for synchronization.
    std::mutex& m_mutex;
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
    // A reference to the MLogger class' vector of handlers.
    std::vector<MLoggerHandler*>& m_handlers;
};

/*
 * The MLogger (Mike-logger) is a thread-safe C++ logger using the iostream operators.
 * To use it, you must invoke a logging level handler which will return an
 * MLoggerEmitter reference, and then terminate your line with std::endl to ensure
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
    MLoggerEmitter& trace();
    // Convenience methods for debug level log
    MLoggerEmitter& debug();
    // Convenience methods for info level log
    MLoggerEmitter& info();
    // Convenience methods for warning level log
    MLoggerEmitter& warn();
    // Convenicence methods for error level log
    MLoggerEmitter& error();
    // Add a new handler. Templated to own the pointer.
    // Example: addHandler<MLoggerStderrHandler>();
    template <typename T>
    void addHandler() {
        T *handler = new T();
        m_handlers.push_back(handler);
    };
    // Clear all handlers.
    void clearHandlers();
    // Initialize the logger with default settings.
    void setDefaults();
private:
    // The logger name.
    std::string m_name;
    // The current log level.
    int m_level;
    // The output stream for the logger.
    std::ostream& m_ostream;
    // The mutex used for synchronization.
    std::mutex m_mutex;
    // Trace handler
    MLoggerEmitter m_trace_handler;
    // Debug handler
    MLoggerEmitter m_debug_handler;
    // Info handler
    MLoggerEmitter m_info_handler;
    // Warn handler
    MLoggerEmitter m_warn_handler;
    // Error handler
    MLoggerEmitter m_error_handler;
    // The thread-safe buffer where the logs are composed.
    std::stringstream m_buffer;
    // A vector of MLoggerHandler* objects.
    std::vector<MLoggerHandler*> m_handlers;
};

#endif /* mlogger_hpp */
