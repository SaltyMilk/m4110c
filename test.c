#include "malloc.h"
#include <stdio.h>

int main()
{
	void *ptr = malloc(4000);
//	printf("1st alloc= %p\n", ptr);
	void *ptrs = malloc(3900);
//	printf("2nd alloc= %p\n", ptrs);
	show_alloc_mem();
	return (0);
}