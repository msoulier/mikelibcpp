//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#include <ctime>
#include <assert.h>
#include <iomanip>

#include "mlogger.hpp"

///////////////////////////////////////////////////////////////////////
// MLogger - A second pass at a C++, thread-safe logger.             //
//     Note that it returns MLoggerEmitter objects to do the actual //
//     logging.                                                      //
///////////////////////////////////////////////////////////////////////

/*
 * The MLoggerHandler base class.
 */
MLoggerHandler::MLoggerHandler()
{}

void MLoggerHandler::operator<< (std::string buffer)
{
    handle(buffer);
}

void MLoggerHandler::handle(std::string buffer)
{
    std::cerr << "This should never be run" << std::endl;
    assert(0);
}

std::string MLoggerHandler::print(void)
{
    return "This should never be run";
}

/*
 * An implementation of an MLoggerHandler that logs to stderr.
 */
MLoggerStderrHandler::MLoggerStderrHandler()
{}

MLoggerStderrHandler::~MLoggerStderrHandler()
{}

void MLoggerStderrHandler::handle(std::string buffer)
{
    std::cerr << buffer << std::endl;
}

std::string MLoggerStderrHandler::print(void)
{
    return "MLoggerStderrHandler";
}

/*
 * An implementation of an MLoggerHandler that logs to a file.
 */
MLoggerFileHandler::MLoggerFileHandler(std::string path,
                                       size_t rotation_filesize,
                                       size_t rotation_filetime,
                                       bool post_compress)
    : m_path(path)
    , m_rotation_filesize(rotation_filesize)
    , m_rotation_filetime(rotation_filetime)
    , m_post_compress(post_compress)
{
}

MLoggerFileHandler::~MLoggerFileHandler()
{}

void MLoggerFileHandler::handle(std::string buffer)
{
}

std::string MLoggerFileHandler::print(void)
{
    std::stringstream result;
    result << "MLoggerFileHandler: "
           << m_path
           << " "
           << m_rotation_filesize
           << " "
           << m_rotation_filetime
           << " "
           << std::boolalpha << m_post_compress;
    return result.str();
}

std::string MLoggerFileHandler::rotation_filesize2s(void)
{
    std::stringstream readable;
    std::string unit = "bytes";
    float result = 0;
    if (m_rotation_filesize > 1024) {
        unit = "kB";
        result = m_rotation_filesize / 1024;
    }
    if (result > 1024) {
        unit = "MB";
        result /= 1024;
    }
    if (result > 1024) {
        unit = "GB";
        result /= 1024;
    }
    readable << result << unit;
    return readable.str();
}

/*
 * The MLoggerEmitters
 */
MLoggerEmitter::MLoggerEmitter(std::stringstream& buffer,
                               std::mutex& mutex,
                               std::ostream& ostream,
                               MLoggerVerbosity threshold,
                               std::string prefix,
                               std::vector<MLoggerHandler*> &handlers)
    : m_buffer(buffer)
    , m_mutex(mutex)
    , m_level(MLoggerVerbosity::info)
    , m_ostream(ostream)
    , m_threshold(threshold)
    , m_prefix(prefix)
    , m_handlers(handlers)
{ }

MLoggerEmitter::~MLoggerEmitter() { }

void MLoggerEmitter::setLevel(MLoggerVerbosity level) {
    m_level = level;
}

MLoggerVerbosity MLogger::getLevel() {
    return m_level;
}

const std::string MLoggerEmitter::localDateTime() {
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
    , m_level(MLoggerVerbosity::info)
    , m_ostream(std::cerr)
    , m_trace_handler(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::trace, "TRACE", m_handlers))
    , m_debug_handler(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::debug, "DEBUG", m_handlers))
    , m_info_handler(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::info, "INFO", m_handlers))
    , m_warn_handler(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::warn, "WARN", m_handlers))
    , m_error_handler(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::error, "ERROR", m_handlers))
    { }

MLogger::MLogger() : MLogger("No name") { }

MLogger::~MLogger() {
    clearHandlers();
}

void MLogger::setLevel(MLoggerVerbosity level) {
    m_level = level;
    // And set it on all of the logger handlers.
    m_trace_handler.setLevel(level);
    m_debug_handler.setLevel(level);
    m_info_handler.setLevel(level);
    m_warn_handler.setLevel(level);
    m_error_handler.setLevel(level);
}

MLoggerEmitter& MLogger::trace() {
    return m_trace_handler;
}

MLoggerEmitter& MLogger::debug() {
    return m_debug_handler;
}

MLoggerEmitter& MLogger::info() {
    return m_info_handler;
}

MLoggerEmitter& MLogger::warn() {
    return m_warn_handler;
}

MLoggerEmitter& MLogger::error() {
    return m_error_handler;
}

std::string MLogger::sprintf(const char *fmt, va_list args)
{
    char buffer[MLOGGER_BUFSIZE];
    // Add to buffer here
    if (vsnprintf(buffer, MLOGGER_BUFSIZE, fmt, args) > MLOGGER_BUFSIZE) {
        fprintf(stderr, "MLogger::printf: output truncated\n");
    }
    std::string msg(buffer);
    return msg;
}

void MLogger::trace(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    m_trace_handler << this->sprintf(fmt, args) << std::endl;
    va_end(args);
}

void MLogger::debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    m_debug_handler << this->sprintf(fmt, args) << std::endl;
    va_end(args);
}

void MLogger::info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    m_info_handler << this->sprintf(fmt, args) << std::endl;
    va_end(args);
}

void MLogger::warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    m_warn_handler << this->sprintf(fmt, args) << std::endl;
    va_end(args);
}

void MLogger::error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    m_error_handler << this->sprintf(fmt, args) << std::endl;
    va_end(args);
}

void MLogger::clearHandlers() {
    // delete all handlers
    for (auto handler : m_handlers) {
        delete handler;
    }
    m_handlers.clear();
}

void MLogger::setDefaults() {
    // Clear any existing handlers.
    clearHandlers();
    // Use a MLoggerStderrHandler

    addHandler<MLoggerStderrHandler>();

    // Defaut log level is info
    setLevel(MLoggerVerbosity::info);
}

const std::vector<MLoggerHandler*>& MLogger::getHandlers() const {
    return m_handlers;
}
