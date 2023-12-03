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
#include <tuple>
#include <errno.h>
#include <type_traits>

#include "to_string.hpp"
#include "mlog.h"
#include "mdebug.h"

#define MLOGGER_BUFSIZE 4096

enum MLoggerVerbosity {
    trace=0,
    debug=10,
    info=20,
    warning=30,
    error=40,
    critical=50
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
    time_t m_rotation_filetime;
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
    void pop_start_time(const char *pathbuf);
    void pop_bytes_written(const char *pathbuf);
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
    MLoggerEmitter& warning();
    // Convenicence methods for error level log with iostream.
    MLoggerEmitter& error();
    // Convenicence methods for critical level log with iostream.
    MLoggerEmitter& critical();
    // Static method for mikelibc to call.
    static void Callback(logseverity_t severity, char *message, void *data);
    // Log printf style at called level.
    template <typename ...Args>
    void trace(Args&&... args) {
        m_trace_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

    template <typename ...Args>
    void debug(Args&&... args) {
        m_debug_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

    template <typename ...Args>
    void info(Args&&... args) {
        m_info_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

    template <typename ...Args>
    void warning(Args&&... args) {
        m_warning_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

    template <typename ...Args>
    void error(Args&&... args) {
        m_error_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

    template <typename ...Args>
    void critical(Args&&... args) {
        m_critical_emitter << this->print(std::forward<Args>(args)...)
                        << std::endl;
    }

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
    MLoggerEmitter m_trace_emitter;
    // Debug handler
    MLoggerEmitter m_debug_emitter;
    // Info handler
    MLoggerEmitter m_info_emitter;
    // Warn handler
    MLoggerEmitter m_warning_emitter;
    // Error handler
    MLoggerEmitter m_error_emitter;
    // Error handler
    MLoggerEmitter m_critical_emitter;
    // The thread-safe buffer where the logs are composed.
    std::stringstream m_buffer;
    // A vector of MLoggerHandler* objects.
    std::vector<MLoggerHandler*> m_handlers;

    // The underlying mikelibc logger handler.
    mlog_handle_t m_handle;

    template<typename ...Args>
    std::string print(const char *fmt, Args &&...args) {
        //mdbgf("in logger print with fmt '%s'\n", fmt);
        std::tuple t = std::make_tuple(to_string<Args>(args)...);
        return std::apply([fmt](auto ...args) {
            //printf(fmt, std::forward<decltype(args)>(args)...);
            //printf("\n");
            char buffer[MLOGGER_BUFSIZE];
            int rv = snprintf(buffer,
                              MLOGGER_BUFSIZE,
                              fmt,
                              std::forward<decltype(args)>(args)...);
            if (rv < 0) {
                perror("sprintf");
                return std::string("");
            } else {
                if (rv > MLOGGER_BUFSIZE) {
                    fprintf(stderr, "MLogger::printf: output truncated\n");
                }
                return std::string(buffer);
            }
        }, t);
    }
};

template <typename T, typename ...Args>
void MLogger::addHandler(Args &&...args) {
    m_handlers.push_back(new T(std::forward<Args>(args)...));
}

#endif /* mlogger_hpp */
