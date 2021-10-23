/*
 * pubkey_to_addrs.c
 * 
 * Copyright 2021 chehw <hongwei.che@gmail.com>
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sha.h"
#include "ripemd.h"
#include "base58.h"
#include "utils.h"
#include "bech32.h"

#include "pubkey_to_addrs.h"

#define COMPRESSED_PUBKEY_SIZE	(33)
#define BITCOIN_ADDR_MAX_SIZE	(100)
#define SHA256_HASH_SIZE 		(32)
#define RIPEMD_HASH_SIZE		(20)


static const char * s_address_types[bitcoin_address_types_count] = {
	[bitcoin_address_type_p2pkh] = "p2pkh",
	[bitcoin_address_type_p2sh_p2pkh] = "p2sh-p2wpkh",
	[bitcoin_address_type_bech32] = "bech32",
};

enum hash_method {
	hash_method_unknown = -1,
	hash_method_hash256,
	hash_method_hash160,
	hash_method_sha256,
	hash_method_ripemd160,
	hash_methods_count
};

static const char * s_methods[hash_methods_count] = {
	"hash256", 
	"hash160", 
	"sha256", 
	"ripemd160", 
};

enum hash_method hash_method_from_string(const char * method)
{
	if(NULL == method) return hash_method_unknown;
	for(int i = 0; i < hash_methods_count; ++i) {
		if(strcasecmp(method, s_methods[i]) == 0) return i;
	}
	return hash_method_unknown;
}

enum bitcoin_address_prefix
{
	bitcoin_address_prefix_p2pkh = 0,
	bitcoin_address_prefix_p2sh = 5,
	bitcoin_address_prefix_privkey = 80,
};

void hash160(const void * data, size_t size, unsigned char hash[static RIPEMD_HASH_SIZE])
{
	unsigned char tmp_hash[SHA256_HASH_SIZE];
	sha256_hash(data, size, tmp_hash);
	ripemd160_hash(tmp_hash, SHA256_HASH_SIZE, hash);
	return;
}
void hash256(const void * data, size_t size, unsigned char hash[static SHA256_HASH_SIZE])
{
	unsigned char tmp_hash[SHA256_HASH_SIZE];
	sha256_hash(data, size, tmp_hash);
	sha256_hash(tmp_hash, SHA256_HASH_SIZE, hash);
	return;
}

enum bitcoin_address_type bitcoin_address_type_from_string(const char * type)
{
	if(NULL == type) return -1;
	for(int i = 0; i < bitcoin_address_types_count; ++i) {
		if(strcmp(type, s_address_types[i]) == 0) return i;
	}
	return -1;
}

const char * bitcoin_address_type_to_string(enum bitcoin_address_type type)
{
	if(type < 0 || type >= bitcoin_address_types_count) return NULL;
	return s_address_types[type];
}

static ssize_t generate_p2pkh_address(const unsigned char pubkey[static COMPRESSED_PUBKEY_SIZE], char **p_addr)
{
	char * addr = *p_addr;
	if(NULL == addr) {
		addr = calloc(BITCOIN_ADDR_MAX_SIZE, 1);
		assert(addr);
		*p_addr = addr;
	}
	
	// step 1. generate ext pubkey data: 
	// [ prefix | hash160(pubkey) | hash256_checksum(4bytes) ]
	unsigned char ext_pubkey[1 + RIPEMD_HASH_SIZE + 4] = { 
		[0] = bitcoin_address_prefix_p2pkh,
	};
	unsigned char checksum[SHA256_HASH_SIZE];
	hash160(pubkey, COMPRESSED_PUBKEY_SIZE, &ext_pubkey[1]);
	hash256(ext_pubkey, 1 + RIPEMD_HASH_SIZE, checksum);
	memcpy(&ext_pubkey[1+ RIPEMD_HASH_SIZE], checksum, 4);
	
	// step2. base58 encode
	return base58_encode(ext_pubkey, 1 + RIPEMD_HASH_SIZE + 4, p_addr);
}

static ssize_t generate_p2sh_p2wpkh_address(const unsigned char pubkey[static COMPRESSED_PUBKEY_SIZE], char ** p_addr) 
{
	char * addr = *p_addr;
	if(NULL == addr) {
		addr = calloc(COMPRESSED_PUBKEY_SIZE, 1);
		assert(addr);
		*p_addr = addr;
	}
	
	// step 1. generate redeem_script (witness program): 
	// [ 0 | <hash_length> | hash160(pubkey) ]
	unsigned char redeem_script[1 + 1 + RIPEMD_HASH_SIZE] = {
		[0] = 0, // p2sh flag
		[1] = 20, // hash length
	};
	hash160(pubkey, COMPRESSED_PUBKEY_SIZE, &redeem_script[2]);
	
	// step 2. generate ext pubkey data: 
	// [ prefix | hash160(redeem_script) | hash256_checksum(4bytes) ]
	unsigned char ext_pubkey[1 + RIPEMD_HASH_SIZE + 4] = { 
		[0] = bitcoin_address_prefix_p2sh,
	};
	unsigned char checksum[SHA256_HASH_SIZE];
	hash160(redeem_script, 2 + RIPEMD_HASH_SIZE, &ext_pubkey[1]);
	hash256(ext_pubkey, 1 + RIPEMD_HASH_SIZE, checksum);
	memcpy(&ext_pubkey[1+ RIPEMD_HASH_SIZE], checksum, 4);
	
	// step3. base58 encode
	return base58_encode(ext_pubkey, 1 + RIPEMD_HASH_SIZE + 4, p_addr);
}

static ssize_t generate_bech32_address(const unsigned char pubkey[static COMPRESSED_PUBKEY_SIZE], char ** p_addr) 
{
	char * addr = *p_addr;
	if(NULL == addr) {
		addr = calloc(100, 1);
		assert(addr);
		*p_addr = addr;
	}
	
	unsigned char hash[RIPEMD_HASH_SIZE] = { 0 };
	hash160(pubkey, COMPRESSED_PUBKEY_SIZE, hash);
	
	return bech32_encode(0, "bc", hash, RIPEMD_HASH_SIZE, addr);
}



static inline int parse_pubkey(const char * pubkey_hex, unsigned char pubkey[static COMPRESSED_PUBKEY_SIZE])
{
	assert(pubkey_hex);
	int cb_pubkey_hex = strlen(pubkey_hex);
	if(cb_pubkey_hex != (COMPRESSED_PUBKEY_SIZE * 2)) {
		fprintf(stderr, "invalid pubkey length: cb=%d, pubkey='%s'.\n", cb_pubkey_hex, pubkey_hex);
		return -1;
	}
	
	void * data = pubkey;
	ssize_t cb = hex2bin(pubkey_hex, cb_pubkey_hex, &data);
	if(cb != 33) {
		fprintf(stderr, "invalid pubkey_hex format: pubkey='%s'.\n", pubkey_hex);
		return -1;
	}
	return 0;
}

ssize_t pubkey_to_p2pkh(const char * pubkey_hex, char ** p_addr)
{
	unsigned char pubkey[COMPRESSED_PUBKEY_SIZE] = { 0 };
	if(0 != parse_pubkey(pubkey_hex, pubkey)) return -1;
	
	return generate_p2sh_p2wpkh_address(pubkey, p_addr);
}

ssize_t pubkey_to_p2sh_p2wpkh(const char * pubkey_hex, char ** p_addr)
{
	unsigned char pubkey[COMPRESSED_PUBKEY_SIZE] = { 0 };
	if(0 != parse_pubkey(pubkey_hex, pubkey)) return -1;
	
	return generate_p2pkh_address(pubkey, p_addr);
}
ssize_t pubkey_to_bech32(const char * pubkey_hex, char ** p_addr)
{
	unsigned char pubkey[COMPRESSED_PUBKEY_SIZE] = { 0 };
	if(0 != parse_pubkey(pubkey_hex, pubkey)) return -1;
	
	return generate_bech32_address(pubkey, p_addr);
}
