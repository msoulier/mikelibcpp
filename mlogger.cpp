//
//  Created by Michael Soulier on 2015-09-26.
//  Copyright © 2015 Michael Soulier. All rights reserved.
//

#include <time.h>
#include <assert.h>
#include <iomanip>
#include <sys/stat.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <functional>

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

std::ostream& operator<< (std::ostream& os, const MLoggerHandler& me) {
    const std::string out = me.identify();
    os << out;
    return os;
}

void MLoggerHandler::handle(std::string buffer)
{
    std::cerr << "This should never be run" << std::endl;
    assert(0);
}

std::string MLoggerHandler::identify(void) const {
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

std::string MLoggerStderrHandler::identify(void) const
{
    std::string response("MLoggerStderrHandler");
    return response;
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
    m_max_path_size = 1024;
    m_logfile = NULL;
    m_start_time.tm_year = 0;
    m_bytes_written = 0;
    // The path should be something like /var/log/foo/foo.log,
    // which needs to be a symlink to the real file,
    // Enforce the .log suffix.
    if (!validate_path(path)) {
        throw std::invalid_argument("paths must end in .log");
    }
    // /var/log/foo/foo-<suffix>.log, where suffix is a datetime
    // stamp.
    setup();
}

MLoggerFileHandler::~MLoggerFileHandler()
{
    if (m_logfile != NULL) {
        fclose(m_logfile);
    }
}

bool MLoggerFileHandler::validate_path(std::string path) {
    // The path must end in .log
    std::string suffix = path.substr(path.size()-4, 4);
    if (suffix == ".log") {
        return true;
    } else {
        return false;
    }
    // Might implement more checks in the future.
}

// Check m_path, if it's a symlink return 1, if it is not
// return 0, if it exists and is not a symlink, return -1.
int MLoggerFileHandler::symlink_exists(void) {
    struct stat filestat;
    /* Stat the file. */
    if (lstat(m_path.c_str(), &filestat) == 0) {
        if (S_ISLNK(filestat.st_mode)) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}

void MLoggerFileHandler::pop_bytes_written(const char *pathbuf) {
    struct stat filestat;
    /* Stat the file. */
    if (stat(pathbuf, &filestat) == 0)
    {
        m_bytes_written = filestat.st_size;
    } else {
        perror("stat");
        throw std::runtime_error("stat on file failed");
    }
}

void MLoggerFileHandler::pop_start_time(const char *pathbuf) {
    std::string spathbuf(pathbuf);
    m_curpath = spathbuf;
    std::string dtstamp = spathbuf.substr(
        spathbuf.size()-4-DATETIMESIZE, DATETIMESIZE
        );
    if (strptime(dtstamp.c_str(), "%Y%m%d%H%M%S", &m_start_time) == NULL) {
        // Thanks to the geniuses that designed strptime, NULL tells us
        // nothing in this case. Could be an error, could be the NULL
        // byte at the end of the input string.
        // If m_start_time.tm_year is still 0 then there was an error.
        if (m_start_time.tm_year == 0) {
            perror("strptime");
            throw std::runtime_error("bad datetime string on file");
        }
    }
}

void MLoggerFileHandler::setup(void) {
    // If there is a symlink at m_path, delete it. Anything else is
    // an error.
    struct stat filestat;
    char pathbuf[m_max_path_size];
    char err[BUFSIZE];
    time_t curtime;
    bool symlink_exists = false;
    /* Stat the file. */
    if (lstat(m_path.c_str(), &filestat) == 0)
    {
        if (S_ISLNK(filestat.st_mode)) {
            symlink_exists = true;
            // Read the link and pull the start time off of
            // the suffix of the file.
            if (readlink(m_path.c_str(), pathbuf, m_max_path_size) < 0) {
                perror("readlink");
                strerror_r(errno, err, BUFSIZE);
                throw std::runtime_error("readlink " + std::string(err));
            }
            // pathbuf now has the path to the real file
            // parse out the datetime stamp
            pop_start_time(pathbuf);
            // And add the file size to bytes_written.
            pop_bytes_written(pathbuf);
        } else {
            // Exists but not a symlink.
            throw std::runtime_error(m_path + " exists and is not a symlink");
        }
    } else {
        // Stat failure. Hopefully doesn't exist.
        m_curpath = getfilename();
        // Set the start time.
        time(&curtime);
        localtime_r(&curtime, &m_start_time);
    }

    // Open the file.
    m_logfile = fopen(m_curpath.c_str(), "a");
    if (m_logfile == NULL) {
        perror("fopen");
        strerror_r(errno, err, BUFSIZE);
        throw std::runtime_error("fopen: " + std::string(err));
    }

    if (! symlink_exists) {
        symlink(m_curpath.c_str(), m_path.c_str());
    }
}

void MLoggerFileHandler::rotate(void) {
    time_t curtime;
    char err[BUFSIZE];

    if (m_logfile != NULL) {
        fclose(m_logfile);
        m_logfile = NULL;
    }
    m_curpath = getfilename();
    // Set the start time.
    time(&curtime);
    localtime_r(&curtime, &m_start_time);
    // zero the written bytes
    m_bytes_written = 0;

    // Open the file.
    m_logfile = fopen(m_curpath.c_str(), "a");
    if (m_logfile == NULL) {
        perror("fopen");
        strerror_r(errno, err, BUFSIZE);
        throw std::runtime_error("fopen: " + std::string(err));
    }

    // Delete the symlink and point it at the new file.
    int exists = symlink_exists();
    if (exists == 1) {
        unlink(m_path.c_str());
    }
    if (exists < 0) {
        throw std::runtime_error(m_path + " exists but is not a symlink");
    }
    symlink(m_curpath.c_str(), m_path.c_str());
}

std::string MLoggerFileHandler::getfilename(void) {
    // Take the existing m_path, remove the .log, append a new
    // time suffix, and append .log again.
    if (m_path.size() < 5) {
        throw std::runtime_error("bad path to logfile, too short");
    }
    std::string subpath = m_path.substr(0, m_path.size()-4);
    subpath += "-" + gettimesuffix() + ".log";
    return subpath;
}

std::string MLoggerFileHandler::gettimesuffix(void) {
    time_t curtime;
    struct tm curtm;
    char buf[DATETIMESIZE+1];

    time(&curtime);
    localtime_r(&curtime, &curtm);
    strftime(buf, DATETIMESIZE+1, "%Y%m%d%H%M%S", &curtm);

    std::string suffix(buf);
    return suffix;
}

void MLoggerFileHandler::handle(std::string buffer)
{
    if (m_logfile != NULL) {
        // Check for rotation by file size.
        m_bytes_written += fprintf(m_logfile, "%s\n", buffer.c_str());
        if (m_bytes_written >= m_rotation_filesize) {
            rotate();
        }
        time_t current_time = time(NULL);
        time_t start_time = mktime(&m_start_time);
        // Check for rotation by file age.
        if (current_time >= (start_time + m_rotation_filetime)) {
            rotate();
        }
    } else {
        fprintf(stderr, "Cannot print to logfile: file not open\n");
    }
}

std::string MLoggerFileHandler::identify(void) const
{
    std::string response("MLoggerFileHandler");
    return response;
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
                               std::vector<std::unique_ptr<MLoggerHandler>> &handlers)
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
    , m_trace_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::trace, "TRACE", m_handlers))
    , m_debug_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::debug, "DEBUG", m_handlers))
    , m_info_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::info, "INFO", m_handlers))
    , m_warning_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::warning, "WARN", m_handlers))
    , m_error_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::error, "ERROR", m_handlers))
    , m_critical_emitter(MLoggerEmitter(m_buffer, m_mutex, m_ostream, MLoggerVerbosity::critical, "CRITICAL", m_handlers))
{
    m_handle = get_mlogger(MLOG_STDERR, MLOG_INFO, LOCNOZONE);
    register_mlog_callback(m_handle, (mlog_cb_t)&Callback, (void *)this);
}

MLogger::MLogger() : MLogger("No name") { }

MLogger::~MLogger() {
    clearHandlers();
    shutdown_mlogger(m_handle);
}

// Static method as a callback for mikelibc's logger to call
void MLogger::Callback(logseverity_t severity, char *message, void *data) {
    MLogger *logger = (MLogger *)data;
    switch (severity) {
        case MLOG_TRACE:
            logger->trace(message);
            break;
        case MLOG_DEBUG:
            logger->debug(message);
            break;
        case MLOG_INFO:
            logger->info(message);
            break;
        case MLOG_WARNING:
            logger->warning(message);
            break;
        case MLOG_ERROR:
            logger->error(message);
            break;
        case MLOG_CRITICAL:
            logger->critical(message);
            break;
        default:
            fprintf(stderr, "unsupported log severity: %s\n", message);
    }
}

MLoggerVerbosity MLogger::getLevel() {
    return m_level;
}

void MLogger::setLevel(MLoggerVerbosity level) {
    m_level = level;
    // And set it on all of the logger handlers.
    m_trace_emitter.setLevel(level);
    m_debug_emitter.setLevel(level);
    m_info_emitter.setLevel(level);
    m_warning_emitter.setLevel(level);
    m_error_emitter.setLevel(level);
    logseverity_t mlog_level = MLOG_TRACE;
    switch (level) {
        case MLoggerVerbosity::trace:
            mlog_level = MLOG_TRACE;
            break;
        case MLoggerVerbosity::debug:
            mlog_level = MLOG_DEBUG;
            break;
        case MLoggerVerbosity::info:
            mlog_level = MLOG_INFO;
            break;
        case MLoggerVerbosity::warning:
            mlog_level = MLOG_WARNING;
            break;
        case MLoggerVerbosity::error:
            mlog_level = MLOG_ERROR;
            break;
        default:
            fprintf(stderr, "unsupported log severity: %d\n", level);
    }
    setloggersev(m_handle, mlog_level);
}

MLoggerEmitter& MLogger::trace() {
    return m_trace_emitter;
}

MLoggerEmitter& MLogger::debug() {
    return m_debug_emitter;
}

MLoggerEmitter& MLogger::info() {
    return m_info_emitter;
}

MLoggerEmitter& MLogger::warning() {
    return m_warning_emitter;
}

MLoggerEmitter& MLogger::error() {
    return m_error_emitter;
}

void MLogger::clearHandlers() {
    // We do not have to delete the handlers, the unique_ptr will take care
    // of that.
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

void MLogger::printHandlers() {
    for (auto&& handler: m_handlers) {
        std::cout << *handler << std::endl;
    }
}

void
    mlog_callback(logseverity_t severity, const char *message, void *data) {
}
