#include "aes256.h"

int main(int argc, char** argv) {
	Key myKey = {
		0x603DEB10,
		0x15CA71BE,
		0x2B73AEF0,
		0x857D7781,
		0x1F352C07,
		0x3B6108D7,
		0x2D9810A3,
		0x0914DFF4
	};
	Block info = {
		0x6BC1BEE2,
		0x2E409F96,
		0xE93D7E11,
		0x7393172A
	};
	
	keyExpansion(myKey);
	printBytes(info);
	cipher(info);
	invCipher(info);
	
	return 0;
}