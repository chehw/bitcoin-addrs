#ifndef CRYPTO_BECH32_H_
#define CRYPTO_BECH32_H_

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

ssize_t bech32_encode(uint8_t version, 
	const char * hrp, // "bc" for mainnet, or "tb" for testnet
	const unsigned char * data, size_t length, // pubkey hash
	char * bech32);



#ifdef __cplusplus
}
#endif
#endif
