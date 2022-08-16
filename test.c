#include "malloc.h"
#include <stdio.h>

int main()
{
	void *ptr = malloc(4000);
//	printf("1st alloc= %p\n", ptr);
	void *ptrs = malloc(3900);
//	printf("2nd alloc= %p\n", ptrs);
	show_alloc_mem();
	printf("---\n");
	free(ptr);
	show_alloc_mem();
	printf("---\n");
	ptr = malloc(4000);
	show_alloc_mem();
	printf("---\n");
	free(ptrs);
	show_alloc_mem();
	printf("---\n");
	free(ptr);
	show_alloc_mem();
	printf("---\n");
	
	
	
	
	free((void *) 0x4242424242424242);
	return (0);
}