#ifndef BITCOIN_ADDRS_PUBKEY_TO_ADDRS_H_
#define BITCOIN_ADDRS_PUBKEY_TO_ADDRS_H_

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

enum bitcoin_address_type
{
	bitcoin_address_type_p2pkh,
	bitcoin_address_type_p2sh_p2pkh,
	bitcoin_address_type_bech32,
	
	bitcoin_address_types_count
};
enum bitcoin_address_type bitcoin_address_type_from_string(const char * type);
const char * bitcoin_address_type_to_string(enum bitcoin_address_type type);

ssize_t pubkey_to_p2pkh(const char * pubkey_hex, char ** p_addr);
ssize_t pubkey_to_p2sh_p2wpkh(const char * pubkey_hex, char ** p_addr);
ssize_t pubkey_to_bech32(const char * pubkey_hex, char ** p_addr);

#ifdef __cplusplus
}
#endif
#endif
