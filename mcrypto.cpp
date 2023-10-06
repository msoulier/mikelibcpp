#include <iostream>
#include <vector>

#include "mcrypto.hpp"

/*
 * Base64Encoder
 */
Base64Encoder::Base64Encoder(void)
{}

Base64Encoder::~Base64Encoder(void)
{}

std::string Base64Encoder::encode(std::string &plaintext)
{
    char *encoded = base64_encode(plaintext.c_str(), plaintext.size());
    if (encoded == NULL) {
        throw std::runtime_error("base64_encode returned a NULL");
    }
    std::string response(encoded);
    free(encoded);
    return response;
}

std::string Base64Encoder::decode(std::string &ciphertext)
{
    char *decoded = base64_decode(ciphertext.c_str(), ciphertext.size());
    if (decoded == NULL) {
        throw std::runtime_error("base64_encode returned a NULL");
    }
    std::string response(decoded);
    free(decoded);
    return response;
}

/*
 * AESEncryptor
 */
AESEncryptor::AESEncryptor(std::string key,
                           std::string iv,
                           const EVP_CIPHER *cipher_type)
    : m_key(key)
    , m_iv(iv)
    , m_cipher_type(cipher_type)
{}

AESEncryptor::~AESEncryptor(void)
{}

std::string AESEncryptor::encrypt(std::string plaintext)
{
    unsigned char *encrypted = encrypt_ssl((unsigned char *)m_key.c_str(),
                                           (unsigned char *)m_iv.c_str(),
                                           m_cipher_type,
                                           (unsigned char *)plaintext.c_str(),
                                           plaintext.size());
    if (encrypted == NULL) {
        throw std::runtime_error("encrypt_aes returned a NULL");
    }
    std::string response((char *)encrypted);
    free(encrypted);
    return response;
}

std::string AESEncryptor::decrypt(std::string ciphertext)
{
    unsigned char *unencrypted = decrypt_ssl((unsigned char *)m_key.c_str(),
                                             (unsigned char *)m_iv.c_str(),
                                             m_cipher_type,
                                             (unsigned char *)ciphertext.c_str(),
                                             ciphertext.size());
    if (unencrypted == NULL) {
        throw std::runtime_error("decrypt_aes returned a NULL");
    }
    std::string response((char *)unencrypted);
    free(unencrypted);
    return response;
}
