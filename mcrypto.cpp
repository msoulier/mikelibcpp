#include <iostream>
#include <vector>
#include <string.h>
#include <cstring>

#include "mcrypto.hpp"
#include "mutil.h"
#include "mdebug.h"

/*
 * Base64Encoder
 */
Base64Encoder::Base64Encoder(void)
{}

Base64Encoder::~Base64Encoder(void)
{}

std::string Base64Encoder::encode(std::basic_string<unsigned char> &data)
{
    char *encoded = base64_encode_openssl(data.data(), data.size());
    if (encoded == NULL) {
        throw std::runtime_error("base64_encode_openssl returned a NULL");
    }
    std::string response(encoded);

    free(encoded);
    return response;
}

std::basic_string<unsigned char> Base64Encoder::decode(std::string &b64string)
{
    size_t output_size = 0;
    unsigned char *decoded = base64_decode_openssl(b64string.c_str(), &output_size);
    mdbgf("b64 decoded b64string, size is %d bytes\n", output_size);
    mdbgf("strlen reports %d bytes\n", strlen((char *)decoded));
    if (decoded == NULL) {
        throw std::runtime_error("base64_encode_openssl returned a NULL");
    }
    std::basic_string<unsigned char> response;
    response.resize( output_size );
    std::memcpy(&response[0], decoded, output_size);

    mdbgf("b64 decoded response is %d long\n", response.size());
    free(decoded);
    return response;
}

/*
 * SHAEncoder
 */
SHAEncoder::SHAEncoder(SHAStrength strength)
    : m_strength(strength)
{
    if (m_strength != SHAStrength::sha1) {
        throw std::runtime_error("only sha1 is supported at this time");
    }
}

SHAEncoder::~SHAEncoder(void) {}

std::basic_string<unsigned char> SHAEncoder::quickdigest(std::basic_string<unsigned char> in) {
    unsigned int cdata_length = 0;
    unsigned char *cdata = digest_sha1(in.c_str(), in.size(), &cdata_length);
    std::basic_string<unsigned char> data(cdata, cdata+cdata_length);
    OPENSSL_free(cdata);
    return data;
}

/*
 * AESEncryptor
 */
AESEncryptor::AESEncryptor(std::basic_string<unsigned char> &key,
                           std::basic_string<unsigned char> &iv,
                           const EVP_CIPHER *cipher_type)
    : m_key(key)
    , m_iv(iv)
    , m_cipher_type(cipher_type)
{}

AESEncryptor::~AESEncryptor(void)
{}

std::basic_string<unsigned char> AESEncryptor::encrypt(std::string &plaintext)
{
    unsigned char *encrypted = encrypt_ssl(m_key.c_str(),
                                           m_iv.c_str(),
                                           m_cipher_type,
                                           (unsigned char *)plaintext.c_str(),
                                           plaintext.size());
    if (encrypted == NULL) {
        throw std::runtime_error("encrypt_aes returned a NULL");
    }
    std::basic_string<unsigned char> response(encrypted);
    free(encrypted);
    return response;
}

std::string AESEncryptor::decrypt(std::basic_string<unsigned char> &ciphertext)
{
    unsigned char *unencrypted = decrypt_ssl(m_key.c_str(),
                                             m_iv.c_str(),
                                             m_cipher_type,
                                             ciphertext.c_str(),
                                             ciphertext.size());
    if (unencrypted == NULL) {
        throw std::runtime_error("decrypt_aes returned a NULL");
    }
    std::string response((char *)unencrypted);
    free(unencrypted);
    return response;
}
