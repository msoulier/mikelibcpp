#ifndef _BASE64_H_
#define _BASE64_H_

/*
 * A simple class to encode/decode a string using base64.
 */
class Base64Encoder
{
public:
    Base64Encoder(void);
    ~Base64Encoder(void);
    std::string encode(std::string &plaintext);
    std::string decode(std::string &ciphertext);
};

/*
 * A simple class to encode/decode a string using AES 256.
 */
class AESEncryptor
{
public:
    AESEncryptor(std::string key, std::string iv);
    ~AESEncryptor(void);
    std::string encrypt(std::string plaintext);
    std::string decrypt(std::string ciphertext);

private:
    std::string m_key;
    std::string m_iv;
};

#endif
