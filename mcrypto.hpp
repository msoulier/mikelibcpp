#ifndef _BASE64_H_
#define _BASE64_H_

typedef unsigned char uchar_t;

std::string base64_encode(uchar_t const* buf, unsigned int bufLen);

std::vector<uchar_t> base64_decode(std::string const& encoded_string);

#endif
