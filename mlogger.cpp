//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#include <ctime>
#include "mlogger.hpp"

//////////////////////////////////////////////////////////////////////
// MLogger - A second pass at a C++, thread-safe logger.            //
//     Note that it returns MLoggerHandler objects to do the actual //
//     logging.                                                     //
//////////////////////////////////////////////////////////////////////

MLoggerHandler::MLoggerHandler(boost::thread_specific_ptr<std::stringstream>& buffer,
                               boost::mutex& mutex,
                               std::ostream& ostream,
                               int threshold,
                               std::string prefix)
    : m_buffer(buffer)
    , m_mutex(mutex)
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
    , m_trace_handler(MLoggerHandler(m_buffer, m_mutex, m_ostream, LOGLEVEL_TRACE, "TRACE"))
    , m_debug_handler(MLoggerHandler(m_buffer, m_mutex, m_ostream, LOGLEVEL_DEBUG, "DEBUG"))
    , m_info_handler(MLoggerHandler(m_buffer, m_mutex, m_ostream, LOGLEVEL_INFO, "INFO"))
    , m_warn_handler(MLoggerHandler(m_buffer, m_mutex, m_ostream, LOGLEVEL_WARN, "WARN"))
    , m_error_handler(MLoggerHandler(m_buffer, m_mutex, m_ostream, LOGLEVEL_ERROR, "ERROR"))
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
