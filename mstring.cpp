#include <iostream>
#include <string>

#include "mstring.hpp"
#include "mutil.h"

MString::MString()
{}

MString::MString(const char *s)
{
    std::string in(s);
    m_string = in;
}

MString::MString(std::string s) : m_string(s)
{}

MString::MString(MString &source)
{
    m_string = source.str();
}

MString& MString::operator=(const MString& source)
{
    m_string = source.m_string;
    return *this;
}

MString::~MString()
{}

std::string MString::str(void)
{
    return m_string;
}

std::vector<std::string> MString::split(std::string on)
{
    std::vector<std::string> splitlist;
    std::string::size_type current_pos = 0;

    while (1) {
        std::string::size_type pos = m_string.find(on, current_pos);
        if (pos == std::string::npos) {
            // Didn't find it
            splitlist.push_back(m_string.substr(current_pos));
            break;
        } else if (pos == 0) {
            splitlist.push_back(std::string(""));
        } else {
            splitlist.push_back(m_string.substr(current_pos, pos-current_pos));
        }
        current_pos = pos + on.size();
    }
    return splitlist;
}

std::string MString::tohex(void) {
    char *c_hex = ::tohex((unsigned char*)m_string.c_str(),
                              m_string.length());
    std::string hex(c_hex);
    free(c_hex);
    return hex;
}

std::ostream& operator<<(std::ostream& os, const MString& me) {
    os << me.m_string;
    return os;
}

// General functions

bool is_shell_metachar(unsigned char c) {
    // Common shell metacharacters
    const std::string metacharacters = ",|&;()<>{}[]$`'\"\\*?~!#^ \t\n/";
    return metacharacters.find(c) != std::string::npos;
}

std::string
sane_elem(std::string& insane)
{
    std::string sane = insane;
    // Convert to lower case.
    std::transform(sane.begin(), sane.end(), sane.begin(),
        [](unsigned char c){ return ::tolower(c); });
    // Convert any consecutive spaces to an underscore.
    std::transform(sane.begin(), sane.end(), sane.begin(),
        [](unsigned char c){ return c == ' ' ? '_' : c; });
    // Screen out any unwanted characters.
    sane.erase(
        std::remove_if(sane.begin(), sane.end(),
            [](unsigned char c){ return c > 127; }),
        sane.end());
    sane.erase(
        std::remove_if(sane.begin(), sane.end(), is_shell_metachar),
        sane.end()
    );

    return sane;
}
