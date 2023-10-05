#include "aes256.h"

unsigned int mod(unsigned int LHS) {
	// Binary modulo using XOR
	// Returns LHS mod 0x11B
	unsigned int remainder = LHS;
	unsigned int temp;
	unsigned char adjustedTemp;
	while (remainder >= 0xFF) {
		temp = 0x11B;
		adjustedTemp = 0;
		while (temp <= remainder) {
			adjustedTemp = 1;
			temp = temp << 1;
		}
		temp = temp >> adjustedTemp;
		remainder = remainder ^ temp;
	}
	return remainder;
}

unsigned char mult_gf256(unsigned char A, unsigned char B) {
	unsigned int xor_sum = 0;
	unsigned char valueIn = A;
	unsigned char temp;
	// Compute the xor sum aka A * B as polynomials
	for (int i = 0; i < 8; ++i) {
		temp = (valueIn % 2 == 1) * 0xFF;
		valueIn = valueIn >> 1;
		xor_sum = xor_sum ^ ((unsigned int)(temp & B) << i);
	}
	// Modulo result by {100011011} aka 0x11B
	temp = mod(xor_sum);
	return temp;
}

void printBytes(Block state) {
	for (int i = 0; i < 4; ++i) {
		printf("%02X %02X %02X %02X\n",state[4*i],state[4*i+1],state[4*i+2],state[4*i+3]);
	}
	printf("\n");
}

void subBytes(Block state) {
	unsigned int temp;
	for (int i = 0; i < 16; ++i) {
		temp = (*(unsigned int*) &state[i]) & 0xFF;
		state[i] = lt_subbytes[temp];
	}
}

void shiftRows(Block state) {
	unsigned char A;
	unsigned char B;
	unsigned char C;
	
	// Nothing todo on first row, do second
	A = state[4];
	state[4] = state[5];
	state[5] = state[6];
	state[6] = state[7];
	state[7] = A;
	// Do third row now
	A = state[8];
	B = state[9];
	state[8] = state[10];
	state[9] = state[11];
	state[10] = A;
	state[11] = B;
	// Do fourth row
	A = state[12];
	B = state[13];
	C = state[14];
	state[12] = state[15];
	state[13] = A;
	state[14] = B;
	state[15] = C;
}

void mixColumns(Block state) {
	unsigned char u3 = 3;
	unsigned char u2 = 2;
	unsigned char temp1;
	unsigned char temp2;
	unsigned char A;
	unsigned char B;
	unsigned char C;
	unsigned char D;
	for (int i = 0; i < 4; ++i) {
		// Save the initial values so new ones can be calculated freely
		A = state[i];
		B = state[4+i];
		C = state[8+i];
		D = state[12+i];
		// First new value
		temp1 = mult_gf256(u2, A);
		temp2 = mult_gf256(u3, B);
		state[i] = temp1 ^ temp2 ^ C ^ D;
		// Second new
		temp1 = mult_gf256(u2, B);
		temp2 = mult_gf256(u3, C);
		state[4+i] = A ^ temp1 ^ temp2 ^ D;
		// Third new
		temp1 = mult_gf256(u2, C);
		temp2 = mult_gf256(u3, D);
		state[8+i] = A ^ B ^ temp1 ^ temp2;
		// Fourth new
		temp1 = mult_gf256(u3, A);
		temp2 = mult_gf256(u2, D);
		state[12+i] = temp1 ^ B ^ C ^ temp2;
	}
}

unsigned int subWord(unsigned int word) {
	for (int i = 0; i < 4; ++i) {
		
	}
}

void keyExpansion(Key userKey) {
	unsigned int i = 0;
	while (i <= (NK - 1)) {
		KeySchedule[4*i] = userKey[4*i];
		KeySchedule[4*i+1] = userKey[4*i+1];
		KeySchedule[4*i+2] = userKey[4*i+2];
		KeySchedule[4*i+3] = userKey[4*i+3];
		i++;
	}
	unsigned int temp;
	while (i <= (4 * NR + 3)) {
		
	}
}