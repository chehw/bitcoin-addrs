/*
 * main.c
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
#include <getopt.h>

#include "pubkey_to_addrs.h"

static void print_usuage(const char * exe_name)
{
	fprintf(stderr, "Usuage: %s pubkey_hex [addr_type]  ## addr_type: [ p2pkh, p2sh-p2wpkh, bech32 ]\n", exe_name);
	fprintf(stderr, "        %s --pubkey=pubkey_hex [--type=addr_type]\n", exe_name);
	return;
}

int parse_args(int argc, char ** argv, char ** p_pubkey_hex, char ** p_addr_type)
{
	static struct option options[] = {
		{"pubkey", required_argument, 0, 'p'},
		{"type", required_argument, 0, 't'},
		{"help", required_argument, 0, 'h'},
	};
	
	char * pubkey_hex = NULL;
	char * addr_type = NULL;
	
	while(1) {
		int option_index = 0;
		int c = getopt_long(argc, argv, "p:t:h", options, &option_index);
		if(c == -1) break;
		
		switch(c) {
		case 'p': pubkey_hex = optarg; break;
		case 't': addr_type = optarg; break;
		case 'h': 
		default:
			print_usuage(argv[0]); return 0;
			exit((c != 'h'));
		}
	}
	
	while(optind < argc) {
		if(NULL == pubkey_hex) {
			pubkey_hex = argv[optind++];
			continue;
		}
		
		if(NULL == addr_type) {
			addr_type = argv[optind++];
			continue;
		}
		
		printf("[WARNING]: unknown non-option args: %s\n", argv[optind++]);
	}
	
	if(NULL == pubkey_hex) {
		print_usuage(argv[0]);
		exit(1);
	}
	
	*p_pubkey_hex = pubkey_hex;
	*p_addr_type = addr_type;
	
	return 0;
}

int main(int argc, char **argv)
{
	char * pubkey_hex = NULL;
	char * addr_type = NULL;
	int rc = 0;
	rc = parse_args(argc, argv, &pubkey_hex, &addr_type);
	assert(0 == rc);
	
	assert(pubkey_hex);
	
	const char * addr_type_p2pkh = bitcoin_address_type_to_string(bitcoin_address_type_p2pkh);
	const char * addr_type_p2sh_p2pkh = bitcoin_address_type_to_string(bitcoin_address_type_p2sh_p2pkh);
	const char * addr_type_bech32 = bitcoin_address_type_to_string(bitcoin_address_type_bech32);
	
	char addr_buf[256] = "";
	char *addr = addr_buf;
	ssize_t cb_addr = 0;
	
	if(NULL == addr_type) {
		cb_addr = pubkey_to_p2pkh(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_p2pkh, addr);
		
		memset(addr_buf, 0, sizeof(addr_buf));
		cb_addr = pubkey_to_p2sh_p2wpkh(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_p2sh_p2pkh, addr);
		
		memset(addr_buf, 0, sizeof(addr_buf));
		cb_addr = pubkey_to_bech32(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_bech32, addr);
		return 0;
	} 
	
	enum bitcoin_address_type type = bitcoin_address_type_from_string(addr_type);
	switch(type) {
	case bitcoin_address_type_p2pkh:
		cb_addr = pubkey_to_p2pkh(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_p2pkh, addr);
		break;
	case bitcoin_address_type_p2sh_p2pkh:
		cb_addr = pubkey_to_p2sh_p2wpkh(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_p2sh_p2pkh, addr);
		break;
	case bitcoin_address_type_bech32:
		cb_addr = pubkey_to_bech32(pubkey_hex, &addr);
		assert(cb_addr > 0);
		printf("[%s addr]: %s\n", addr_type_bech32, addr);
		break;
	default:
		fprintf(stderr, "unknown addr_type: '%s'\n", addr_type);
		return -1;
	}

	return 0;
}
