#include "malloc.h"
#include <stdio.h>

int main()
{
	void *ptr = malloc(4);
	printf("\nsizeof=%lu\n", (4096/100) - (sizeof(size_t) + sizeof(char)));	
	return (0);
}