#ifndef LIBAES_C
#define LIBAES_C

#include "libaes.h"

// PURGES MEMORY
void mult_gf256(uint8 A, uint8 B, uint8* res) {
	uint8 temp;
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
	uint8 A;
	uint8 B;
	uint8 C;
	uint8 D;
	for (uint32 i = 0; i < NB; ++i) {
		A = *((uint8*)(&state[0]) + (3 - i));
		B = *((uint8*)(&state[1]) + (3 - i));
		C = *((uint8*)(&state[2]) + (3 - i));
		D = *((uint8*)(&state[3]) + (3 - i));
		printf("%02X %02X %02X %02X\n",A,B,C,D);
	}
	printf("\n");
	
	// Data purge
	A = 0; B = 0; C = 0; D = 0;
}

// NO MEMORY
void subBytes(Block state) {
	uint8* wordAddr;
	for (uint32 i = 0; i < NB; ++i) {
		wordAddr = (uint8*)(&state[i]); // base address
		// Order doens't matter luckily
		*wordAddr = lt_subbytes[*wordAddr];
		*(wordAddr + 1) = lt_subbytes[*(wordAddr + 1)];
		*(wordAddr + 2) = lt_subbytes[*(wordAddr + 2)];
		*(wordAddr + 3) = lt_subbytes[*(wordAddr + 3)];
	}
}

// NO MEMORY
void invSubBytes(Block state) {
	uint8* wordAddr;
	for (uint32 i = 0; i < NB; ++i) {
		wordAddr = (uint8*)(&state[i]);
		*wordAddr = lt_invsubbytes[*wordAddr];
		*(wordAddr + 1) = lt_invsubbytes[*(wordAddr + 1)];
		*(wordAddr + 2) = lt_invsubbytes[*(wordAddr + 2)];
		*(wordAddr + 3) = lt_invsubbytes[*(wordAddr + 3)];
	}
}

// PURGES MEMORY
void shiftRows(Block state) {
	uint8 A;
	uint8 B;
	uint8 C;
	// Base word addresses
	uint8* word1 = (uint8*)(&state[0]);
	uint8* word2 = (uint8*)(&state[1]);
	uint8* word3 = (uint8*)(&state[2]);
	uint8* word4 = (uint8*)(&state[3]);
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
	uint8 A;
	uint8 B;
	uint8 C;
	// Base word addresses
	uint8* word1 = (uint8*)(&state[0]);
	uint8* word2 = (uint8*)(&state[1]);
	uint8* word3 = (uint8*)(&state[2]);
	uint8* word4 = (uint8*)(&state[3]);
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
	uint8 A; uint8 temp1;
	uint8 B; uint8 temp2;
	uint8 C;
	uint8 D;
	uint8* word;
	for (uint32 i = 0; i < NB; ++i) {
		word = (uint8*)(&state[i]);
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
	uint8 A; uint8 temp1;
	uint8 B; uint8 temp2;
	uint8 C; uint8 temp3;
	uint8 D; uint8 temp4;
	uint8* word;
	for (uint32 i = 0; i < NB; ++i) {
		word = (uint8*)(&state[i]);
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
void subWord(uint32 word, uint32* res) {
	uint32 returnVal;
	uint8 tempC;
	// i=0 is for LS byte
	// i=3 is for MS byte
	for (uint32 i = 0; i < 4; ++i) {
		tempC = *((uint8*)(&word) + i); // Get the current byte
		*((uint8*)(&returnVal) + i) = lt_subbytes[tempC]; // Update it
	}
	
	// Set output value
	*res = returnVal;
	// Data purge
	tempC = 0; returnVal = 0;
}

// PURGES MEMORY
void rotWord(uint32 word, uint32* res) {
	uint32 returnVal;
	// Calculate base addresses once since they're used a lot
	uint8* returnAddr = (uint8*)(&returnVal);
	uint8* wordAddr = (uint8*)(&word);
	// Do the byte swaps
	uint8 tempC = *(wordAddr + 3); // MS byte
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
	uint32 i = 0;
	uint32 tempWord;
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

// PURGES MEMORY
void addRoundKey(Block state, uint8 round) {
	uint8 temp = NB*round;
	state[0] = state[0] ^ KeySchedule[temp];
	state[1] = state[1] ^ KeySchedule[temp + 1];
	state[2] = state[2] ^ KeySchedule[temp + 2];
	state[3] = state[3] ^ KeySchedule[temp + 3];
	// Data purge
	temp = 0;
}

// NO MEMORY
void cipher(Block state) {
	addRoundKey(state, 0);
	for (uint8 round = 1; round < NR; ++round) {
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
	for (uint8 round = NR - 1; round >= 1; --round) {
		invShiftRows(state);
		invSubBytes(state);
		addRoundKey(state, round);
		invMixColumns(state);
	}
	invShiftRows(state);
	invSubBytes(state);
	addRoundKey(state, 0);
}

void findExtension(char str[FILENAME_SIZE], char** addr) {
	char ext[] = ".aes";
	uint8 matchPlace = 0;
	char* tempAddr;
	//uint8 found = 0;
	for (uint8 i = 0; i < 100; ++i) {
		if (matchPlace >= 4) {
			*addr = tempAddr;
			return;
		}
		if (str[i] == '.') { tempAddr = &str[i]; }
		if (str[i] == ext[matchPlace]) {
			matchPlace++;
		} else {
			matchPlace = 0;
		}
	}
	// Set return value
	*addr = 0;
	// Data purge
	tempAddr = 0; matchPlace = 0;
}

// MAIN FUNCTIONS
// PURGES MEMORY
void fileInterface(char filename[], Key userKey, uint8 mode) {
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
		//char *needleStart = strstr(temp, ".aes"); // Including string.h for this
		char* needleStart;
		findExtension(temp, &needleStart);
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
	if (!outputFile) {
		printf("libAES: Error opening file '%s'\n",outFilename);
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

	uint8 buf[DATABUF_SIZE];	// I/O buffer
	int byteCount = 0;			// How many bytes in block so far
	uint8 currentByte;
	uint32 tempWord;			// Temporary word for building a block
	uint8* baseAddr;			// The base address of the current word
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
			baseAddr = (uint8*)(&tempWord);
			for (uint32 i = 0; i < NB; ++i) {
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
			for (uint32 w = 0; w < NB; ++w) {
				baseAddr = (uint8*)(&data[w]);
				fputc(*(baseAddr + 3), outputFile);
				fputc(*(baseAddr + 2), outputFile);
				fputc(*(baseAddr + 1), outputFile);
				fputc(*baseAddr, outputFile);
			}
		}
	}
	if (byteCount != 0) {
		// Build the data block
		baseAddr = (uint8*)(&tempWord);
		for (uint32 i = 0; i < NB; ++i) {
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
		for (uint32 w = 0; w < NB; ++w) {
			baseAddr = (uint8*)(&data[w]);
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
	for (uint32 i = 0; i < DATABUF_SIZE; ++i) { buf[i] = 0; } // Buffer
	for (uint32 i = 0; i < NB; ++i) { data[i] = 0; } // Data block
	// Key schedule
	for (uint32 i = 0; i < KEY_SCH_WORDS; ++i) { KeySchedule[i] = 0; }
	tempWord = 0; currentByte = 0; byteCount = 0; // Misc variables
	resetKey(userKey); // Never leave key in memory
}

void printKey(Key userKey) {
	printf("libAES: Key contents:\n");
	for (uint32 i = 0; i < NK; ++i) {
		printf("%08lX\n",userKey[i]);
	}
}

void resetKey(Key userKey) {
	for (uint32 i = 0; i < NK; ++i) {
		userKey[i] = 0;
	}
}

// PURGES MEMORY
int importKey(char keyAsText[], Key userKey) {
	// Check key length and composition
	uint32 keyLen = (uint32)(strlen(keyAsText));
	if (keyLen != 64) {
		resetKey(userKey);
		keyLen = 0;
		return -1;
	}

	uint32 tempWord;
	char temp;
	for (uint32 w = 0; w < NK; ++w) {
		tempWord = 0;
		for (uint32 hb = 0; hb < 8; ++hb) {
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