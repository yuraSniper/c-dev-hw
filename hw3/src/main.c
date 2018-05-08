#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "alloc.h"

int main(int argc, char * argv[])
{
	if (!mstart())
	{
		fprintf(stderr, "Error: Couldn't initialize the newAlloc!\n");
		exit(1);
	}

	char * str1 = alloc(sizeof(char) * 256);
	int * blah = alloc(sizeof(int) * 8);
	int * my_int = alloc(sizeof(int) * 1);
	uint64_t * uint64 = alloc(sizeof(uint64_t) * 6);

	char * str2 = alloc(sizeof(char) * 195);
	char * str3 = alloc(sizeof(char) * 204);

	printf("str1: %p\n", (void *) str1);
	printf("blah: %p\n", (void *) blah);
	printf("my_int: %p\n", (void *) my_int);
	printf("uint64: %p\n", (void *) uint64);
	printf("str2: %p\n", (void *) str2);
	printf("str3: %p\n", (void *) str3);

	memset(str1, 'a', 255);
	memset(str2, 'b', 194);
	memset(str3, 'c', 203);

	str1[255] = '\0';
	str2[194] = '\0';
	str3[203] = '\0';

	printf("str1: %s\n\n", str1);
	printf("str2: %s\n\n", str2);
	printf("str3: %s\n\n", str3);

	mfree(blah);
	mfree(my_int);
	mfree(uint64);
	mfree(str2);
	mfree(str1);
	mfree(str3);

	mstop();
	return 0;
}
