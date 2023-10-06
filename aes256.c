#include "aes256.h"

unsigned int mod(unsigned int LHS) {
	unsigned int remainder = LHS;
	unsigned int temp;
	unsigned int modRHS;
	unsigned int bits;
	while (remainder >= 0x100) {
		temp = 0x100;
		bits = 9;
		modRHS = 0x11B;
		while (temp <= remainder) {
			temp = temp << 1;
			bits++;
		}
		modRHS = modRHS << ((bits - 10)*(bits > 9));
		remainder = remainder ^ modRHS;
	}
	return remainder;
}

unsigned char mult_gf256(unsigned char A, unsigned char B) {
	unsigned int xor_sum = 0;
	unsigned char valueIn = A;
	unsigned char temp;
	// Compute the xor sum aka A * B as polynomials
	for (unsigned int i = 0; i < 8; ++i) {
		temp = (valueIn % 2 == 1) * 0xFF;
		valueIn = valueIn >> 1;
		xor_sum = xor_sum ^ ((unsigned int)(temp & B) << i);
	}
	// Modulo result by {100011011} aka 0x11B
	temp = mod(xor_sum);
	return temp;
}

void printBytes(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	for (unsigned int i = 0; i < 4; ++i) {
		A = *((unsigned char*)(&state[0]) + (3-i));
		B = *((unsigned char*)(&state[1]) + (3-i));
		C = *((unsigned char*)(&state[2]) + (3-i));
		D = *((unsigned char*)(&state[3]) + (3-i));
		printf("%02X %02X %02X %02X\n",A,B,C,D);
	}
	printf("\n");
}

void subBytes(Block state) {
	unsigned char temp;
	unsigned char* wordAddr;
	for (unsigned int i = 0; i < 4; ++i) {
		wordAddr = (unsigned char*)(&state[i]); // base address
		// Order doens't matter luckily
		*wordAddr = lt_subbytes[*wordAddr];
		*(wordAddr + 1) = lt_subbytes[*(wordAddr + 1)];
		*(wordAddr + 2) = lt_subbytes[*(wordAddr + 2)];
		*(wordAddr + 3) = lt_subbytes[*(wordAddr + 3)];
	}
}

void invSubBytes(Block state) {
	unsigned char temp;
	unsigned char* wordAddr;
	for (unsigned int i = 0; i < 4; ++i) {
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
	for (unsigned int i = 0; i < 4; ++i) {
		word = (unsigned char*)(&state[i]);
		// Save the initial values so new ones can be calculated freely
		A = *(word + 3);
		B = *(word + 2);
		C = *(word + 1);
		D = *word;
		// First new value
		*(word + 3) = mult_gf256(0x2, A) ^ mult_gf256(0x3, B) ^ C ^ D;
		// Second new
		*(word + 2) = A ^ mult_gf256(0x2, B) ^ mult_gf256(0x3, C) ^ D;
		// Third new
		*(word + 1) = A ^ B ^ mult_gf256(0x2, C) ^ mult_gf256(0x3, D);
		// Fourth new
		*word = mult_gf256(0x3, A) ^ B ^ C ^ mult_gf256(0x2, D);
	}
}

void invMixColumns(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	unsigned char* word;
	for (unsigned int i = 0; i < 4; ++i) {
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
	for (unsigned int i = 0; i < 4; ++i) {
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
	while (i <= (4 * NR + 3)) {
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
	for (unsigned int i = 0; i < 4; ++i) {
		state[i] = state[i] ^ KeySchedule[4*round + i];
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
	printBytes(state);
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
	printBytes(state);
}