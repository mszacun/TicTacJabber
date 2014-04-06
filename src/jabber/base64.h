#include <cstring>
#include <cstdlib>

#ifndef BASE64_H
#define BASE64_H

char *base64_encode(const char *buf, int len);
char *base64_decode(const char *buf);

#endif
