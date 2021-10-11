//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#include <ctime>
#include "mlogger.hpp"

//////////////////////////////////////////////////////////////////////
// SimpleLogger - A first pass at a thread-safe logger.             //
//////////////////////////////////////////////////////////////////////

SimpleLogger::SimpleLogger() : m_level(LOGLEVEL_INFO), m_ostream(std::cerr)
{

}

SimpleLogger::~SimpleLogger() {
    
}

void SimpleLogger::setLevel(int level) {
    m_level = level;
}

int SimpleLogger::getLevel() {
    return m_level;
}

const std::string SimpleLogger::localDateTime() {
    const char *format = "%b %d %Y @ %X %Z";
    std::time_t t = std::time(NULL);
    char buffer[128];
    if (std::strftime(buffer, sizeof(buffer), format, std::localtime(&t))) {
        return std::string(buffer);
    }
    else {
        return "";
    }
}

void SimpleLogger::log(int level, std::string msg) {
    if (level >= m_level) {
        m_mutex.lock();
        switch (level) {
            case LOGLEVEL_TRACE:
                m_ostream << "[TRACE] ";
                break;
            case LOGLEVEL_DEBUG:
                m_ostream << "[DEBUG] ";
                break;
            case LOGLEVEL_INFO:
                m_ostream << "[INFO] ";
                break;
            case LOGLEVEL_WARN:
                m_ostream << "[WARNING] ";
                break;
            case LOGLEVEL_ERROR:
                m_ostream << "[ERROR] ";
                break;
            default:
                m_ostream << "[" << level << "] ";
        }
        m_ostream << localDateTime() << " ";
        m_ostream << msg << std::endl;
        m_mutex.unlock();
    }
}

void SimpleLogger::trace(const char *msg) {
    this->log(LOGLEVEL_TRACE, std::string(msg));
}

void SimpleLogger::trace(std::string msg) {
    this->log(LOGLEVEL_TRACE, msg);
}

void SimpleLogger::debug(const char *msg) {
    this->log(LOGLEVEL_DEBUG, std::string(msg));
}

void SimpleLogger::debug(std::string msg) {
    this->log(LOGLEVEL_DEBUG, msg);
}

void SimpleLogger::info(const char *msg) {
    this->log(LOGLEVEL_INFO, std::string(msg));
}

void SimpleLogger::info(std::string msg) {
    this->log(LOGLEVEL_INFO, msg);
}

void SimpleLogger::warn(const char *msg) {
    this->log(LOGLEVEL_WARN, std::string(msg));
}

void SimpleLogger::warn(std::string msg) {
    this->log(LOGLEVEL_WARN, msg);
}

void SimpleLogger::error(const char *msg) {
    this->log(LOGLEVEL_ERROR, std::string(msg));
}

void SimpleLogger::error(std::string msg) {
    this->log(LOGLEVEL_ERROR, msg);
}

//////////////////////////////////////////////////////////////////////
// MLogger - A second pass at a C++, thread-safe logger.            //
//     Note that it returns MLoggerHandler objects to do the actual //
//     logging.                                                     //
//////////////////////////////////////////////////////////////////////

boost::thread_specific_ptr<std::stringstream> tls_buffer;

MLoggerHandler::MLoggerHandler(boost::mutex& mutex,
                               std::ostream& ostream,
                               int threshold,
                               std::string prefix)
    : m_mutex(mutex)
    , m_level(LOGLEVEL_INFO)
    , m_ostream(ostream)
    , m_threshold(threshold)
    , m_prefix(prefix)
{ }

MLoggerHandler::~MLoggerHandler() { }

void MLoggerHandler::setLevel(int level) {
    m_level = level;
}

const std::string MLoggerHandler::localDateTime() {
    const char *format = "%b %d %Y @ %X %Z";
    std::time_t t = std::time(NULL);
    char buffer[128];
    if (std::strftime(buffer, sizeof(buffer), format, std::localtime(&t))) {
        return std::string(buffer);
    }
    else {
        return "";
    }
}

MLogger::MLogger(std::string name)
    : m_name(name)
    , m_level(LOGLEVEL_INFO)
    , m_ostream(std::cerr)
    , m_trace_handler(MLoggerHandler(m_mutex, m_ostream, LOGLEVEL_TRACE, "TRACE"))
    , m_debug_handler(MLoggerHandler(m_mutex, m_ostream, LOGLEVEL_DEBUG, "DEBUG"))
    , m_info_handler(MLoggerHandler(m_mutex, m_ostream, LOGLEVEL_INFO, "INFO"))
    , m_warn_handler(MLoggerHandler(m_mutex, m_ostream, LOGLEVEL_WARN, "WARN"))
    , m_error_handler(MLoggerHandler(m_mutex, m_ostream, LOGLEVEL_ERROR, "ERROR"))
    { }

MLogger::MLogger() : MLogger("No name") { }

MLogger::~MLogger() { }

int MLogger::getLevel() {
    return m_level;
}

void MLogger::setLevel(int level) {
    m_level = level;
    // And set it on all of the logger handlers.
    m_trace_handler.setLevel(level);
    m_debug_handler.setLevel(level);
    m_info_handler.setLevel(level);
    m_warn_handler.setLevel(level);
    m_error_handler.setLevel(level);
}

MLoggerHandler& MLogger::trace() {
    return m_trace_handler;
}

MLoggerHandler& MLogger::debug() {
    return m_debug_handler;
}

MLoggerHandler& MLogger::info() {
    return m_info_handler;
}

MLoggerHandler& MLogger::warn() {
    return m_warn_handler;
}

MLoggerHandler& MLogger::error() {
    return m_error_handler;
}
