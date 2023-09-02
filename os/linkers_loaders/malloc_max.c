#include <stdio.h>
#include <stdlib.h>

unsigned maximum = 0;

int main() {
	unsigned blocks[] = {1024 * 1024 * 1024, 1024 * 1024, 1 * 1024};
	int i, count;
	for (i = 0; i < 3; ++i) {
		for (count = i; ; count++) {
			void *block = malloc(maximum + blocks[i] * count);
			printf("alloc block size = %p \n", block);
			if (block) {
				maximum = maximum + blocks[i] * count;
				//free(block);
			} else break;
		}
	}
	printf("maximum malloc size = %u bytes\n", maximum);



	return 0;
}
