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


	mstop();
	return 0;
}
