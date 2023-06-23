#include <iostream>
#include <string>

#include "mstring.hpp"

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

std::ostream& operator<<(std::ostream& os, const MString& me) {
    os << me.m_string;
    return os;
}
