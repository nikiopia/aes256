#include "libaes.h"
#include <time.h>

int main(int argc, char** argv) {
	Key myKey;
	time_t startTime;
	time_t finishTime;

	// argv[0]: exe name, argv[1]: mode e/d,
	// argv[2]: file to process, argv[3]: key
	if (argc == 4) {
		char mode = argv[1][0];
		if (mode != 'e' && mode != 'E' &&
			mode != 'd' && mode != 'D') { return 0; }
		printf("libAES: Checking key...\n");
		if (importKey(argv[3], myKey) < 0) {
			printf("libAES: Key error, length and/or composition\n");
			printKey(myKey);
			return 0;
		}
		if (mode == 'e' || mode == 'E') {
			printf("libAES: Beginning encryption on '%s'\n",argv[2]);
		} else {
			printf("libAES: Beginning decryption on '%s'\n",argv[2]);
		}
		startTime = time(NULL);
		fileInterface(argv[2], myKey, (mode == 'd' || mode == 'D'));
		finishTime = time(NULL);
		finishTime = finishTime - startTime;
		printf("libAES: Done, Took %lld(s)\n",finishTime);
	}

	printKey(myKey);
	return 0;
}