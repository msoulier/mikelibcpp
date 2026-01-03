#ifndef MSTRING_HPP
#define MSTRING_HPP

#include <vector>
#include <string>
#include <algorithm>

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
    std::string tohex(void);

    friend std::ostream& operator<<(std::ostream& os, const MString& me);

private:
    std::string m_string;
};

bool is_shell_metachar(unsigned char c);

std::string
sane_elem(std::string& insane);
    // Test
    //std::string insane = "Shitty Windows File&Name ∞ (too long)";
    //std::cout << sane_elem(insane) << std::endl;
    //exit(0);

#endif
