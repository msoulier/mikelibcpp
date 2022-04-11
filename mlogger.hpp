//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#ifndef mlogger_hpp
#define mlogger_hpp

#define BUFSIZE 1024
#define DATETIMESIZE 14 // ie. 20220409181030

#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <vector>
#include <stdio.h>
#include <stdarg.h>

#define MLOGGER_BUFSIZE 1024

enum MLoggerVerbosity {
    trace=0,
    debug=10,
    info=20,
    warn=30,
    error=40
};

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
    // For iostream operations to the handler (ie. logs)
    void operator<< (std::string buffer);
    virtual std::string print(void);
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
    std::string print(void);
private:
    void handle(std::string buffer);
};

/*
 * A rotating file handler.
 */
class MLoggerFileHandler: public MLoggerHandler
{
public:
    MLoggerFileHandler(std::string path,
                       size_t rotation_filesize,
                       size_t rotation_filetime,
                       bool post_compress);
    ~MLoggerFileHandler();
    std::string print(void);
private:
    // The path to the log symlink
    std::string m_path;
    // The path to the current open logfile
    std::string m_curpath;
    size_t m_rotation_filesize;
    size_t m_rotation_filetime;
    bool m_post_compress;
    FILE *m_logfile;
    size_t m_max_path_size;
    struct tm m_start_time;
    size_t m_bytes_written;

    void handle(std::string buffer);
    std::string rotation_filesize2s(void);
    std::string gettimesuffix(void);
    std::string getfilename(void);
    void setup(void);
    bool validate_path(std::string path);
    void rotate(void);
    int symlink_exists(void);
};

class MLoggerEmitter
{
public:
    MLoggerEmitter(std::stringstream& buffer,
                   std::mutex& mutex,
                   std::ostream& ostream,
                   MLoggerVerbosity threshold,
                   std::string prefix,
                   std::vector<MLoggerHandler*> &handlers);
    ~MLoggerEmitter();
    void setLevel(MLoggerVerbosity level);

    template <class T>
    // For handling << from any object.
    MLoggerEmitter& operator <<(T input) {
        // Only log if the level is set above our threshold.
        if (m_threshold >= m_level) {
            std::lock_guard<std::mutex> lock(m_mutex);
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
    MLoggerVerbosity m_level;
    // The output stream.
    std::ostream& m_ostream;
    // The threshold for logging for this handler.
    MLoggerVerbosity m_threshold;
    // The string prefix for logging.
    std::string m_prefix;
    // Return the current date and time as a localized string.
    const std::string localDateTime();
    // A reference to the MLogger class' vector of handlers.
    std::vector<MLoggerHandler*>& m_handlers;
};

/*
 * The MLogger (Mike-logger) is a thread-safe C++ logger using the iostream
 * operators.  To use it, you must invoke a logging level handler which will
 * return an MLoggerEmitter reference, and then terminate your line with
 * std::endl to ensure that the buffer is flushed and the line terminated
 * with a newline.
 */
class MLogger
{
public:
    MLogger();
    MLogger(std::string name);
    ~MLogger();
    // Block copy constructor and assignment operator.
    MLogger(MLogger& source) = delete;
    MLogger& operator=(const MLogger& source) = delete;
    // Set the current logging level
    void setLevel(MLoggerVerbosity level);
    // Get the current logging level
    MLoggerVerbosity getLevel();
    // Convenience methods for trace level log with iostream.
    MLoggerEmitter& trace();
    // Convenience methods for debug level log with iostream.
    MLoggerEmitter& debug();
    // Convenience methods for info level log with iostream.
    MLoggerEmitter& info();
    // Convenience methods for warning level log with iostream.
    MLoggerEmitter& warn();
    // Convenicence methods for error level log with iostream.
    MLoggerEmitter& error();
    // Log printf style at called level.
    void trace(const char *fmt, ...);
    void debug(const char *fmt, ...);
    void info(const char *fmt, ...);
    void warn(const char *fmt, ...);
    void error(const char *fmt, ...);

    template <typename T, typename ...Args>
    void addHandler(Args &&...args);

    // Clear all handlers.
    void clearHandlers();
    // Initialize the logger with default settings.
    void setDefaults();
    // Return the handlers
    const std::vector<MLoggerHandler*>& getHandlers() const;
private:
    // The logger name.
    std::string m_name;
    // The current log level.
    MLoggerVerbosity m_level;
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
    // A print method for handling va_args in one place.
    std::string sprintf(const char *fmt, va_list args);
};

template <typename T, typename ...Args>
void MLogger::addHandler(Args &&...args) {
    m_handlers.push_back(new T(std::forward<Args>(args)...));
}

#endif /* mlogger_hpp */
