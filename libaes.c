#ifndef LIBAES_C
#define LIBAES_C

#include "libaes.h"

// PURGES MEMORY
void mult_gf256(unsigned char A, unsigned char B, unsigned char* res) {
	/*
	//XTimes(b) = (b << 1) ^ ((0x1B) * (b >= 0x80));
	unsigned char cur_xtimes = A;		// This value must be updated each cycle
	// If LSB(B)=1 -> result=A, else result=0 (initially)
	unsigned char result = A * (B & 1);
	unsigned char bitQueue = B >> 1; 	// Holds bits left to process
	while (bitQueue > 0) {
		cur_xtimes = (unsigned char)(cur_xtimes << 1) ^ 
			(0x1B * (cur_xtimes >= 0x80));
		result = result ^ (cur_xtimes * (bitQueue & 1));
		bitQueue = bitQueue >> 1;
	}
	
	// Set output value
	*res = result;
	// Data purge
	cur_xtimes = 0; bitQueue = 0; result = 0;
	*/
	unsigned char temp;
	switch(A) {
		case 2:
			temp = 0;
			break;
		case 3:
			temp = 1;
			break;
		case 9:
			temp = 2;
			break;
		case 0xB:
			temp = 3;
			break;
		case 0xD:
			temp = 4;
			break;
		case 0xE:
			temp = 5;
			break;
	}
	// Return value
	*res = lt_gf256[temp][B];
	// Data purge
	temp = 0;
}

// PURGES MEMORY
void printBytes(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	for (unsigned int i = 0; i < NB; ++i) {
		A = *((unsigned char*)(&state[0]) + (3 - i));
		B = *((unsigned char*)(&state[1]) + (3 - i));
		C = *((unsigned char*)(&state[2]) + (3 - i));
		D = *((unsigned char*)(&state[3]) + (3 - i));
		printf("%02X %02X %02X %02X\n",A,B,C,D);
	}
	printf("\n");
	
	// Data purge
	A = 0; B = 0; C = 0; D = 0;
}

// NO MEMORY
void subBytes(Block state) {
	unsigned char* wordAddr;
	for (unsigned int i = 0; i < NB; ++i) {
		wordAddr = (unsigned char*)(&state[i]); // base address
		// Order doens't matter luckily
		*wordAddr = lt_subbytes[*wordAddr];
		*(wordAddr + 1) = lt_subbytes[*(wordAddr + 1)];
		*(wordAddr + 2) = lt_subbytes[*(wordAddr + 2)];
		*(wordAddr + 3) = lt_subbytes[*(wordAddr + 3)];
	}
}

// NO MEMORY
void invSubBytes(Block state) {
	unsigned char* wordAddr;
	for (unsigned int i = 0; i < NB; ++i) {
		wordAddr = (unsigned char*)(&state[i]);
		*wordAddr = lt_invsubbytes[*wordAddr];
		*(wordAddr + 1) = lt_invsubbytes[*(wordAddr + 1)];
		*(wordAddr + 2) = lt_invsubbytes[*(wordAddr + 2)];
		*(wordAddr + 3) = lt_invsubbytes[*(wordAddr + 3)];
	}
}

// PURGES MEMORY
void shiftRows(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	// Base word addresses
	unsigned char* word1 = (unsigned char*)(&state[0]);
	unsigned char* word2 = (unsigned char*)(&state[1]);
	unsigned char* word3 = (unsigned char*)(&state[2]);
	unsigned char* word4 = (unsigned char*)(&state[3]);
	// Second row
	A = *(word1 + 2);
	*(word1 + 2) = *(word2 + 2);
	*(word2 + 2) = *(word3 + 2);
	*(word3 + 2) = *(word4 + 2);
	*(word4 + 2) = A;
	// Third row
	A = *(word1 + 1);
	B = *(word2 + 1);
	*(word1 + 1) = *(word3 + 1);
	*(word2 + 1) = *(word4 + 1);
	*(word3 + 1) = A;
	*(word4 + 1) = B;
	// Fourth row
	A = *word1;
	B = *word2;
	C = *word3;
	*word1 = *word4;
	*word2 = A;
	*word3 = B;
	*word4 = C;
	
	// Data purge
	A = 0; B = 0; C = 0;
}

// PURGES MEMORY
void invShiftRows(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	// Base word addresses
	unsigned char* word1 = (unsigned char*)(&state[0]);
	unsigned char* word2 = (unsigned char*)(&state[1]);
	unsigned char* word3 = (unsigned char*)(&state[2]);
	unsigned char* word4 = (unsigned char*)(&state[3]);
	// Second row
	A = *(word4 + 2);
	*(word4 + 2) = *(word3 + 2);
	*(word3 + 2) = *(word2 + 2);
	*(word2 + 2) = *(word1 + 2);
	*(word1 + 2) = A;
	// Third row
	A = *(word4 + 1);
	B = *(word3 + 1);
	*(word4 + 1) = *(word2 + 1);
	*(word3 + 1) = *(word1 + 1);
	*(word2 + 1) = A;
	*(word1 + 1) = B;
	// Fourth row
	A = *word4;
	B = *word3;
	C = *word2;
	*word4 = *word1;
	*word3 = A;
	*word2 = B;
	*word1 = C;
	
	// Data purge
	A = 0; B = 0; C = 0;
}

// PURGES MEMORY
void mixColumns(Block state) {
	unsigned char A; unsigned char temp1;
	unsigned char B; unsigned char temp2;
	unsigned char C;
	unsigned char D;
	unsigned char* word;
	for (unsigned int i = 0; i < NB; ++i) {
		word = (unsigned char*)(&state[i]);
		// Save the initial values so new ones can be calculated freely
		A = *(word + 3);
		B = *(word + 2);
		C = *(word + 1);
		D = *word;
		// First new value
		mult_gf256(2, A, &temp1);
		mult_gf256(3, B, &temp2);
		*(word + 3) = temp1 ^ temp2 ^ C ^ D;
		// Second new
		mult_gf256(2, B, &temp1);
		mult_gf256(3, C, &temp2);
		*(word + 2) = A ^ temp1 ^ temp2 ^ D;
		// Third new
		mult_gf256(2, C, &temp1);
		mult_gf256(3, D, &temp2);
		*(word + 1) = A ^ B ^ temp1 ^ temp2;
		// Fourth new
		mult_gf256(3, A, &temp1);
		mult_gf256(2, D, &temp2);
		*word = temp1 ^ B ^ C ^ temp2;
	}

	// Data purge
	A = 0; B = 0; C = 0; D = 0;
	temp1 = 0; temp2 = 0;
}

// PURGES MEMORY
void invMixColumns(Block state) {
	unsigned char A; unsigned char temp1;
	unsigned char B; unsigned char temp2;
	unsigned char C; unsigned char temp3;
	unsigned char D; unsigned char temp4;
	unsigned char* word;
	for (unsigned int i = 0; i < NB; ++i) {
		word = (unsigned char*)(&state[i]);
		A = *(word + 3);
		B = *(word + 2);
		C = *(word + 1);
		D = *word;
		// First new value
		mult_gf256(0xE, A, &temp1);
		mult_gf256(0xB, B, &temp2);
		mult_gf256(0xD, C, &temp3);
		mult_gf256(0x9, D, &temp4);
		*(word + 3) = temp1 ^ temp2 ^ temp3 ^ temp4;
		// Second new value
		mult_gf256(0x9, A, &temp1);
		mult_gf256(0xE, B, &temp2);
		mult_gf256(0xB, C, &temp3);
		mult_gf256(0xD, D, &temp4);
		*(word + 2) = temp1 ^ temp2 ^ temp3 ^ temp4;
		// Third new value
		mult_gf256(0xD, A, &temp1);
		mult_gf256(0x9, B, &temp2);
		mult_gf256(0xE, C, &temp3);
		mult_gf256(0xB, D, &temp4);
		*(word + 1) = temp1 ^ temp2 ^ temp3 ^ temp4;
		// Fourth new value
		mult_gf256(0xB, A, &temp1);
		mult_gf256(0xD, B, &temp2);
		mult_gf256(0x9, C, &temp3);
		mult_gf256(0xE, D, &temp4);
		*word = temp1 ^ temp2 ^ temp3 ^ temp4;
	}

	// Data purge
	A = 0; B = 0; C = 0; D = 0;
	temp1 = 0; temp2 = 0; temp3 = 0; temp4 = 0;
}

// PURGES MEMORY
void subWord(unsigned int word, unsigned int* res) {
	unsigned int returnVal;
	unsigned char tempC;
	// i=0 is for LS byte
	// i=3 is for MS byte
	for (unsigned int i = 0; i < NB; ++i) {
		tempC = *((unsigned char*)(&word) + i); // Get the current byte
		*((unsigned char*)(&returnVal) + i) = lt_subbytes[tempC]; // Update it
	}
	
	// Set output value
	*res = returnVal;
	// Data purge
	tempC = 0; returnVal = 0;
}

// PURGES MEMORY
void rotWord(unsigned int word, unsigned int* res) {
	unsigned int returnVal;
	// Calculate base addresses once since they're used a lot
	unsigned char* returnAddr = (unsigned char*)(&returnVal);
	unsigned char* wordAddr = (unsigned char*)(&word);
	// Do the byte swaps
	unsigned char tempC = *(wordAddr + 3); // MS byte
	*(returnAddr + 3) = *(wordAddr + 2);
	*(returnAddr + 2) = *(wordAddr + 1);
	*(returnAddr + 1) = *wordAddr;
	*returnAddr = tempC;

	// Set output value
	*res = returnVal;
	// Data purge
	tempC = 0; returnVal = 0;
}

// PURGES MEMORY
void keyExpansion(Key userKey) {
	unsigned int i = 0;
	unsigned int tempWord;
	while (i <= (NK - 1)) {
		KeySchedule[i] = userKey[i];
		i++;
	}
	while (i <= (NB * NR + 3)) {
		tempWord = KeySchedule[i-1];
		if (i % NK == 0) {
			rotWord(tempWord, &tempWord);
			subWord(tempWord, &tempWord);
			tempWord = tempWord ^ Rcon[i/NK - 1];
			//tempWord = subWord(rotWord(tempWord)) ^ Rcon[i/NK - 1];
		} else if (NK > 6 && i % NK == 4) {
			subWord(tempWord, &tempWord);
			//tempWord = subWord(tempWord);
		}
		KeySchedule[i] = KeySchedule[i-NK] ^ tempWord;
		i++;
	}
	
	// Data purge
	i = 0; tempWord = 0;
}

// NO MEMORY
void addRoundKey(Block state, unsigned int round) {
	for (unsigned int i = 0; i < NB; ++i) {
		state[i] = state[i] ^ KeySchedule[NB*round + i];
	}
}

// NO MEMORY
void cipher(Block state) {
	addRoundKey(state, 0);
	for (unsigned int round = 1; round < NR; ++round) {
		subBytes(state);
		shiftRows(state);
		mixColumns(state);
		addRoundKey(state, round);
	}
	subBytes(state);
	shiftRows(state);
	addRoundKey(state, NR);
}

// NO MEMORY
void invCipher(Block state) {
	addRoundKey(state, NR);
	for (unsigned int round = NR - 1; round >= 1; --round) {
		invShiftRows(state);
		invSubBytes(state);
		addRoundKey(state, round);
		invMixColumns(state);
	}
	invShiftRows(state);
	invSubBytes(state);
	addRoundKey(state, 0);
}

// MAIN FUNCTIONS
// PURGES MEMORY
void fileInterface(char filename[], Key userKey, unsigned int mode) {
	if (mode != 0 && mode != 1) { return; }

	char inFilename[FILENAME_SIZE];
	char outFilename[FILENAME_SIZE];
	// Input is always as given
	sprintf(inFilename, "%s", filename);
	// Determine output filename
	if (mode == 0) {
		// Adds .aes
		sprintf(outFilename, "%s.aes", filename);
	} else {
		// "Removes" .aes
		char temp[FILENAME_SIZE];
		sprintf(temp, "%s", filename); // Make a local copy to be able to edit
		char *needleStart = strstr(temp, ".aes"); // Including string.h for this
		if (!needleStart) {
			printf("libAES: No .aes extension (%s), was this file encryped?\n",
				filename);
			return;
		}
		*needleStart = 0;
		sprintf(outFilename, "%s", temp); // Move local copy to output name var
	}

	FILE* inputFile = fopen(inFilename, "rb");
	FILE* outputFile = fopen(outFilename, "wb");
	if (!inputFile) {
		printf("libAES: Error opening file '%s'\n",inFilename);
		return;
	}

	// Set the action function pointer based on mode
	// mode = 0: encyption
	//      = 1: decryption
	void (*actionFunc)(Block);
	if (mode == 0) {
		actionFunc = &cipher;
	} else {
		actionFunc = &invCipher;
	}

	unsigned char buf[FILEBUF_SIZE];	// I/O buffer
	int byteCount = 0;					// How many bytes in block so far
	unsigned char currentByte;
	unsigned int tempWord;				// Temporary word for building a block
	unsigned char* baseAddr;			// The base address of the current word
	Block data;
	
	keyExpansion(userKey);
	
	while (1) {
		currentByte = fgetc(inputFile);
		if (feof(inputFile)) { break; }
		buf[byteCount] = currentByte;
		byteCount++;
		
		if (byteCount == 16) {
			byteCount = 0;
			// Build the data block
			baseAddr = (unsigned char*)(&tempWord);
			for (unsigned int i = 0; i < NB; ++i) {
				// Build the word
				*(baseAddr + 3) = buf[NB*i];
				*(baseAddr + 2) = buf[NB*i + 1];
				*(baseAddr + 1) = buf[NB*i + 2];
				*baseAddr = buf[NB*i + 3];
				// Reset the buffer as we go
				buf[NB*i] = 0;
				buf[NB*i + 1] = 0;
				buf[NB*i + 2] = 0;
				buf[NB*i + 3] = 0;
				// Save it
				data[i] = tempWord;
			}
			// Process it
			(*actionFunc)(data);
			// Save it to the output file
			for (unsigned int w = 0; w < NB; ++w) {
				baseAddr = (unsigned char*)(&data[w]);
				fputc(*(baseAddr + 3), outputFile);
				fputc(*(baseAddr + 2), outputFile);
				fputc(*(baseAddr + 1), outputFile);
				fputc(*baseAddr, outputFile);
			}
		}
	}
	if (byteCount != 0) {
		// Build the data block
		baseAddr = (unsigned char*)(&tempWord);
		for (unsigned int i = 0; i < NB; ++i) {
			// Build the word
			*(baseAddr + 3) = buf[NB*i];
			*(baseAddr + 2) = buf[NB*i + 1];
			*(baseAddr + 1) = buf[NB*i + 2];
			*baseAddr = buf[NB*i + 3];
			// Save it
			data[i] = tempWord;
		}
		// Process it
		(*actionFunc)(data);
		// Save it to the output file
		for (unsigned int w = 0; w < NB; ++w) {
			baseAddr = (unsigned char*)(&data[w]);
			fputc(*(baseAddr + 3), outputFile);
			fputc(*(baseAddr + 2), outputFile);
			fputc(*(baseAddr + 1), outputFile);
			fputc(*baseAddr, outputFile);
		}
	}
	
	// Close files
	fclose(inputFile);
	fclose(outputFile);
	// Purge all data
	for (unsigned int i = 0; i < FILEBUF_SIZE; ++i) { buf[i] = 0; } // Buffer
	for (unsigned int i = 0; i < NB; ++i) { data[i] = 0; } // Data block
	// Key schedule
	for (unsigned int i = 0; i < KEY_SCH_WORDS; ++i) { KeySchedule[i] = 0; }
	tempWord = 0; currentByte = 0; byteCount = 0; // Misc variables
	resetKey(userKey); // Never leave key in memory
}

void printKey(Key userKey) {
	printf("libAES: Key contents:\n");
	for (unsigned int i = 0; i < NK; ++i) {
		printf("%08X\n",userKey[i]);
	}
}

void resetKey(Key userKey) {
	for (unsigned int i = 0; i < NK; ++i) {
		userKey[i] = 0;
	}
}

// PURGES MEMORY
int importKey(char keyAsText[], Key userKey) {
	// Check key length and composition
	unsigned int keyLen = (unsigned int)(strlen(keyAsText));
	if (keyLen != 64) {
		resetKey(userKey);
		return -1;
	}

	unsigned int tempWord;
	char temp;
	for (unsigned int w = 0; w < NK; ++w) {
		tempWord = 0;
		for (unsigned int hb = 0; hb < 8; ++hb) {
			temp = keyAsText[8*w + hb];
			if (temp >= 48 && temp <= 57) {
				// 0-9
				tempWord = (tempWord << 4) + (temp - 48);
			} else if (temp >= 65 && temp <= 70) {
				// A-F
				tempWord = (tempWord << 4) + (temp - 55);
			} else if (temp >= 97 && temp <= 102) {
				// a-f
				tempWord = (tempWord << 4) + (temp - 87);
			} else {
				// Key processing failed, purge key from memory
				resetKey(userKey);
				return -1;
			}
		}
		userKey[w] = tempWord;
	}
	
	// Data purge
	tempWord = 0; temp = 0; keyLen = 0;
	
	return 0;
}

#endif