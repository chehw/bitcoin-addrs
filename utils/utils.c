/*
 * utils.c
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
#include <errno.h>
#include <stdint.h>

#include "utils.h"

static const char s_hex_digits[256 * 2 + 1] = 
	"00" "01" "02" "03" "04" "05" "06" "07"    "08" "09" "0a" "0b" "0c" "0d" "0e" "0f" 
	"10" "11" "12" "13" "14" "15" "16" "17"    "18" "19" "1a" "1b" "1c" "1d" "1e" "1f" 
	"20" "21" "22" "23" "24" "25" "26" "27"    "28" "29" "2a" "2b" "2c" "2d" "2e" "2f" 
	"30" "31" "32" "33" "34" "35" "36" "37"    "38" "39" "3a" "3b" "3c" "3d" "3e" "3f" 
	"40" "41" "42" "43" "44" "45" "46" "47"    "48" "49" "4a" "4b" "4c" "4d" "4e" "4f" 
	"50" "51" "52" "53" "54" "55" "56" "57"    "58" "59" "5a" "5b" "5c" "5d" "5e" "5f" 
	"60" "61" "62" "63" "64" "65" "66" "67"    "68" "69" "6a" "6b" "6c" "6d" "6e" "6f" 
	"70" "71" "72" "73" "74" "75" "76" "77"    "78" "79" "7a" "7b" "7c" "7d" "7e" "7f" 
	"80" "81" "82" "83" "84" "85" "86" "87"    "88" "89" "8a" "8b" "8c" "8d" "8e" "8f" 
	"90" "91" "92" "93" "94" "95" "96" "97"    "98" "99" "9a" "9b" "9c" "9d" "9e" "9f" 
	"a0" "a1" "a2" "a3" "a4" "a5" "a6" "a7"    "a8" "a9" "aa" "ab" "ac" "ad" "ae" "af" 
	"b0" "b1" "b2" "b3" "b4" "b5" "b6" "b7"    "b8" "b9" "ba" "bb" "bc" "bd" "be" "bf" 
	"c0" "c1" "c2" "c3" "c4" "c5" "c6" "c7"    "c8" "c9" "ca" "cb" "cc" "cd" "ce" "cf" 
	"d0" "d1" "d2" "d3" "d4" "d5" "d6" "d7"    "d8" "d9" "da" "db" "dc" "dd" "de" "df" 
	"e0" "e1" "e2" "e3" "e4" "e5" "e6" "e7"    "e8" "e9" "ea" "eb" "ec" "ed" "ee" "ef" 
	"f0" "f1" "f2" "f3" "f4" "f5" "f6" "f7"    "f8" "f9" "fa" "fb" "fc" "fd" "fe" "ff" 
;

static const unsigned char s_hex_table[256] = { 
/* 0x00 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x10 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x20 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x30 */	   0,    1,    2,    3,    4,    5,    6,    7,        8,    9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x40 */	0xff,   10,   11,   12,   13,   14,   15, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x50 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x60 */	0xff,   10,   11,   12,   13,   14,   15, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x70 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

/* 0x80 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x90 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xa0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xb0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xc0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xd0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xe0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0xf0 */	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

int8_t hexdigit(unsigned char c) 
{ 
	uint8_t value = s_hex_table[c];
	if(value == 0xff) return -1;
	return (int8_t)value;
}


ssize_t bin2hex(const void * data, size_t length, char ** p_hex)
{
	static const uint16_t * digits = (uint16_t *)s_hex_digits;
	if(length == 0 || NULL == data) return 0;
	ssize_t size = length * 2;
	if(NULL == p_hex) return size + 1;
	const unsigned char * p = data;
	char * hex = *p_hex;
	if(NULL == hex)
	{
		hex = malloc(size + 1);
		if(NULL == hex) return -1;
		hex[size] = '\0';
		*p_hex = hex;
	}
	
	uint16_t * dst = (uint16_t *)hex;
	for(size_t i = 0; i < length; ++i) dst[i] = digits[p[i]];
	return size;
}

ssize_t hex2bin(const char * hex, size_t length, void ** p_data)
{
	if(NULL == hex) return 0;
	if(((ssize_t)length) <= 0) length = strlen(hex);
	if(length == 0) return 0;
	if(length % 2) return -1;
	
	ssize_t size = length / 2;
	if(NULL == p_data) return size;	// return buffer_size
	
	unsigned char * data = * p_data;
	if(NULL == data)
	{
		data = malloc(size);
		assert(data);
		if(NULL == data) return -1;
	}

	for(size_t i = 0; i < size; ++i)
	{
		unsigned char hi = s_hex_table[(int)hex[i * 2]];
		unsigned char lo = s_hex_table[(int)hex[i * 2 + 1]];
		if(hi > 0x0F || lo > 0x0F) goto label_err;
		data[i] = (hi << 4) | lo;
	}

	*p_data = data;
	return size;
label_err:
	if(NULL == *p_data) free(data);
	return -1;
}
