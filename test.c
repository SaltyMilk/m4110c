#include "malloc.h"
#include <stdio.h>

int main()
{
	void *ptr = malloc(4000);
	//void *ptrs = malloc(3900);
	show_alloc_mem();
	return (0);
}