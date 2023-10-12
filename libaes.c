#include "libaes.h"

unsigned char mult_gf256(unsigned char A, unsigned char B) {
	//XTimes(b) = (b << 1) ^ ((0x1B) * (b >= 0x80));
	unsigned char cur_xtimes = A;		// this value needs updating for each cycle
	unsigned char result = A * (B & 1); // if LSB(B)=1 -> result=A, else result=0 (initially)
	unsigned char bitQueue = B >> 1; 	// holds all of the bits left to process
	while (bitQueue > 0) {
		cur_xtimes = (unsigned char)(cur_xtimes << 1) ^ (0x1B * (cur_xtimes >= 0x80));
		result = result ^ (cur_xtimes * (bitQueue & 1));
		bitQueue = bitQueue >> 1;
	}
	return result;
}

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
}

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
}

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
}

void mixColumns(Block state) {
	unsigned char A;
	unsigned char B;
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
		*(word + 3) = mult_gf256(2, A) ^ mult_gf256(3, B) ^ C ^ D;
		// Second new
		*(word + 2) = A ^ mult_gf256(2, B) ^ mult_gf256(3, C) ^ D;
		// Third new
		*(word + 1) = A ^ B ^ mult_gf256(2, C) ^ mult_gf256(3, D);
		// Fourth new
		*word = mult_gf256(3, A) ^ B ^ C ^ mult_gf256(2, D);
	}
}

void invMixColumns(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	unsigned char* word;
	for (unsigned int i = 0; i < NB; ++i) {
		word = (unsigned char*)(&state[i]);
		A = *(word + 3);
		B = *(word + 2);
		C = *(word + 1);
		D = *word;
		// First new value
		*(word + 3) = mult_gf256(0xE, A) ^ mult_gf256(0xB, B) ^ 
			mult_gf256(0xD, C) ^ mult_gf256(0x9, D);
		// Second new value
		*(word + 2) = mult_gf256(0x9, A) ^ mult_gf256(0xE, B) ^
			mult_gf256(0xB, C) ^ mult_gf256(0xD, D);
		// Third new value
		*(word + 1) = mult_gf256(0xD, A) ^ mult_gf256(0x9, B) ^
			mult_gf256(0xE, C) ^ mult_gf256(0xB, D);
		// Fourth new value
		*word = mult_gf256(0xB, A) ^ mult_gf256(0xD, B) ^
			mult_gf256(0x9, C) ^ mult_gf256(0xE, D);
	}
}

unsigned int subWord(unsigned int word) {
	unsigned int returnVal;
	unsigned char tempC;
	// i=0 is for LS byte
	// i=3 is for MS byte
	for (unsigned int i = 0; i < NB; ++i) {
		tempC = *((unsigned char*)(&word) + i); // get the current byte
		*((unsigned char*)(&returnVal) + i) = lt_subbytes[tempC]; // update it
	}
	return returnVal;
}

unsigned int rotWord(unsigned int word) {
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
	return returnVal;
}

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
			tempWord = subWord(rotWord(tempWord)) ^ Rcon[i/NK - 1];
		} else if (NK > 6 && i % NK == 4) {
			tempWord = subWord(tempWord);
		}
		KeySchedule[i] = KeySchedule[i-NK] ^ tempWord;
		i++;
	}
}

void addRoundKey(Block state, unsigned int round) {
	for (unsigned int i = 0; i < NB; ++i) {
		state[i] = state[i] ^ KeySchedule[NB*round + i];
	}
}

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
		sprintf(temp, "%s", filename); // Need to make a local copy to be able to edit
		char *needleStart = strstr(temp, ".aes"); // The sole reason for importing string.h
		if (!needleStart) {
			printf("libAES: No .aes extension (%s), was this file encryped?\n", filename);
			return;
		}
		*needleStart = 0;
		sprintf(outFilename, "%s", temp); // Move the local copy to output name string
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

	unsigned char buf[FILEBUF_SIZE];	// I/O buffer
	int byteCount = 0;					// How many bytes have been read for the current block
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

	fclose(inputFile);
	fclose(outputFile);
}

void printKey(Key theKey) {
	for (unsigned int i = 0; i < NK; ++i) {
		printf("%08X\n",theKey[i]);
	}
}

int importKey(char keyAsText[], Key userKey) {
	// Check key length and composition
	unsigned int keyLen = (unsigned int)(strlen(keyAsText));
	if (keyLen != 64) { return -1; }

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
				return -1;
			}
		}
		userKey[w] = tempWord;
	}
	return 0;
}