#ifndef _BASE64_H_
#define _BASE64_H_

class Base64Encoder
{
public:
    Base64Encoder(void);
    ~Base64Encoder(void);
    std::string encode(std::string &plaintext);
    std::string decode(std::string &ciphertext);
};

#endif
