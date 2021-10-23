#ifndef BITCOIN_ADDRS_UTILS_H_
#define BITCOIN_ADDRS_UTILS_H_

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

int8_t hexdigit(unsigned char c);
ssize_t bin2hex(const void * data, size_t length, char ** p_hex);
ssize_t hex2bin(const char * hex, size_t length, void ** p_data);

#ifdef __cplusplus
}
#endif
#endif

