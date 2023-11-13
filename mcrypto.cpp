#include <iostream>
#include <vector>
#include <string.h>
#include <cstring>

#include "mcrypto.hpp"
#include "mdebug.h"

/*
 * Base64Encoder
 */
Base64Encoder::Base64Encoder(void)
{}

Base64Encoder::~Base64Encoder(void)
{}

std::string Base64Encoder::encode(std::vector<unsigned char> &data)
{
    size_t encoded_size = 0;
    char *encoded = base64_encode((const unsigned char*)data.data(), data.size(), &encoded_size);
    if (encoded == NULL) {
        throw std::runtime_error("base64_encode returned a NULL");
    }
    std::string response(encoded, encoded_size);

    free(encoded);
    return response;
}

std::vector<unsigned char> Base64Encoder::decode(std::string &b64string)
{
    size_t output_size = 0;
    unsigned char *decoded = base64_decode(b64string.c_str(), b64string.size(), &output_size);
    mdbgf("b64 decoded b64string, size is %d bytes\n", output_size);
    mdbgf("strlen reports %d bytes\n", strlen((char *)decoded));
    if (decoded == NULL) {
        throw std::runtime_error("base64_encode returned a NULL");
    }
    std::vector<unsigned char> response;
    response.resize( output_size );
    std::memcpy(&response[0], decoded, output_size);

    mdbgf("b64 decoded response is %d long\n", response.size());
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
