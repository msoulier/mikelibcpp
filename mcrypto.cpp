#include <iostream>
#include <vector>

#include "mcrypto.hpp"
#include "mutil.h"

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
    return response;
}

std::string Base64Encoder::decode(std::string &ciphertext)
{
    char *decoded = base64_decode(ciphertext.c_str(), ciphertext.size());
    if (decoded == NULL) {
        throw std::runtime_error("base64_encode returned a NULL");
    }
    std::string response(decoded);
    return response;
}
