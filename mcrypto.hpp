#ifndef _BASE64_H_
#define _BASE64_H_

#include <openssl/evp.h>

/*
 * A simple class to encode/decode a string using base64.
 */
class Base64Encoder
{
public:
    Base64Encoder(void);
    ~Base64Encoder(void);
    std::string encode(std::vector<unsigned char> &data);
    std::vector<unsigned char> decode(std::string &b64string);
};

/*
 * A simple class to encode/decode a string using the provided cipher.
 * See EVP_CIPHER(3ssl) for all cipher types. If cipher_type is NULL,
 * EVP_aes_256_cfb8() will be used.
 */
class AESEncryptor
{
public:
    AESEncryptor(std::basic_string<unsigned char> key,
                 std::basic_string<unsigned char> iv,
                 const EVP_CIPHER *cipher_type = NULL);
    ~AESEncryptor(void);
    std::basic_string<unsigned char> encrypt(std::string plaintext);
    std::string decrypt(std::basic_string<unsigned char> ciphertext);

private:
    std::basic_string<unsigned char> m_key;
    std::basic_string<unsigned char> m_iv;
    const EVP_CIPHER *m_cipher_type;
};

#endif
