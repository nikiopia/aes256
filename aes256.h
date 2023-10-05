#ifndef AES256_H
#define AES256_H

// FIPS 197 has specification for AES-256
// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf

// DEFINES
#define NB				4  // # of 32-bit words per block
#define NK				8  // # of 32-bit words per key
#define NR				14 // # of rounds
#define BLOCK_BYTES 	16 // NB * 4 bytes = 16 bytes -> 128 bits
#define KEY_BYTES		32 // NK * 4 bytes = 32 bytes -> 256 bits
#define KEY_SCH_WORDS	60 // 4 * (NR + 1) = 60 words

// TYPEDEFS
typedef unsigned char Block[BLOCK_BYTES];
typedef unsigned char Key[KEY_BYTES];

// LOOKUP TABLES
unsigned char lt_subbytes[256] = {
'\x63','\x7c','\x77','\x7b','\xf2','\x6b','\x6f','\xc5','\x30','\x01','\x67','\x2b','\xfe','\xd7','\xab','\x76',
'\xca','\x82','\xc9','\x7d','\xfa','\x59','\x47','\xf0','\xad','\xd4','\xa2','\xaf','\x9c','\xa4','\x72','\xc0',
'\xb7','\xfd','\x93','\x26','\x36','\x3f','\xf7','\xcc','\x34','\xa5','\xe5','\xf1','\x71','\xd8','\x31','\x15',
'\x04','\xc7','\x23','\xc3','\x18','\x96','\x05','\x9a','\x07','\x12','\x80','\xe2','\xeb','\x27','\xb2','\x75',
'\x09','\x83','\x2c','\x1a','\x1b','\x6e','\x5a','\xa0','\x52','\x3b','\xd6','\xb3','\x29','\xe3','\x2f','\x84',
'\x53','\xd1','\x00','\xed','\x20','\xfc','\xb1','\x5b','\x6a','\xcb','\xbe','\x39','\x4a','\x4c','\x58','\xcf',
'\xd0','\xef','\xaa','\xfb','\x43','\x4d','\x33','\x85','\x45','\xf9','\x02','\x7f','\x50','\x3c','\x9f','\xa8',
'\x51','\xa3','\x40','\x8f','\x92','\x9d','\x38','\xf5','\xbc','\xb6','\xda','\x21','\x10','\xff','\xf3','\xd2',
'\xcd','\x0c','\x13','\xec','\x5f','\x97','\x44','\x17','\xc4','\xa7','\x7e','\x3d','\x64','\x5d','\x19','\x73',
'\x60','\x81','\x4f','\xdc','\x22','\x2a','\x90','\x88','\x46','\xee','\xb8','\x14','\xde','\x5e','\x0b','\xdb',
'\xe0','\x32','\x3a','\x0a','\x49','\x06','\x24','\x5c','\xc2','\xd3','\xac','\x62','\x91','\x95','\xe4','\x79',
'\xe7','\xc8','\x37','\x6d','\x8d','\xd5','\x4e','\xa9','\x6c','\x56','\xf4','\xea','\x65','\x7a','\xae','\x08',
'\xba','\x78','\x25','\x2e','\x1c','\xa6','\xb4','\xc6','\xe8','\xdd','\x74','\x1f','\x4b','\xbd','\x8b','\x8a',
'\x70','\x3e','\xb5','\x66','\x48','\x03','\xf6','\x0e','\x61','\x35','\x57','\xb9','\x86','\xc1','\x1d','\x9e',
'\xe1','\xf8','\x98','\x11','\x69','\xd9','\x8e','\x94','\x9b','\x1e','\x87','\xe9','\xce','\x55','\x28','\xdf',
'\x8c','\xa1','\x89','\x0d','\xbf','\xe6','\x42','\x68','\x41','\x99','\x2d','\x0f','\xb0','\x54','\xbb','\x16'
};

unsigned char lt_invsubbytes[256];

unsigned int Rcon[10] = {
0x01000000,
0x02000000,
0x04000000,
0x08000000,
0x10000000,
0x20000000,
0x40000000,
0x80000000,
0x1B000000,
0x36000000
};

// KEY SCHEDULE
unsigned int KeySchedule[KEY_SCH_WORDS];

// FUNCTION PROTOTYPES
unsigned int mod(unsigned int LHS);
unsigned char mult_gf256(unsigned char A, unsigned char B);
void printBytes(Block state);
unsigned int subWord(unsigned int word);
unsigned int rotWord(unsigned int word);

void subBytes(Block state);
void shiftRows(Block state);
void mixColumns(Block state);
//void addRoundKey(Block state);
void keyExpansion(Key userKey);
//void cipher(Block state, int nr, 

// INCLUDES
#include <stdio.h>
#include "aes256.c"

#endif