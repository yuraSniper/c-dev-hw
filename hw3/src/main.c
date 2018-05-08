#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "alloc.h"

int main(int argc, char * argv[])
{
	if (!mstart())
	{
		fprintf(stderr, "Error: Couldn't initialize the newAlloc!\n");
		exit(1);
	}
	
	/*char * str = alloc(sizeof(char) * 256);
	int * blah = alloc(sizeof(int) * 8);
	int * my_int = alloc(sizeof(int) * 1);
	uint64_t * uint64 = alloc(sizeof(uint64_t) * 6);

	mfree(blah);
	mfree(my_int);
	mfree(uint64);
	mfree(str);
*/
	mstop();
	return 0;
}
