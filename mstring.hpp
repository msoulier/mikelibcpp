#ifndef MSTRING_HPP
#define MSTRING_HPP

#include <vector>

class MString
{
public:
    MString();
    MString(const char *s);
    MString(std::string s);
    MString(MString &source);
    MString& operator=(const MString& source);
    ~MString();
    std::string str(void);
    std::vector<std::string> split(std::string on);

    friend std::ostream& operator<<(std::ostream& os, const MString& me);

private:
    std::string m_string;
};

#endif
