#include "libaes.h"

int main(int argc, char** argv) {
	Key myKey;

	if (argc == 4) {
		if (argv[1][0] == 'e') {
			printf("libAES: Checking key...\n");
			printf("key input=%s\n",argv[3]);
			if (importKey(argv[3], myKey) < 0) {
				printf("libAES: Key error, length or composition\n");
				printKey(myKey);
				return 0;
			}
			printf("libAES: Beginning encryption on '%s'\n",argv[2]);
			fileInterface(argv[2], myKey, 0);
			printf("Done.\n");
		} else if (argv[1][0] == 'd') {
			printf("libAES: Checking key...\n");
			if (importKey(argv[3], myKey) < 0) {
				printf("libAES: Key error, length or composition\n");
				printKey(myKey);
				return 0;
			}
			printf("libAES: Beginning decryption on '%s'\n",argv[2]);
			fileInterface(argv[2], myKey, 1);
			printf("Done.\n");
		}
	}

	return 0;
}