#include "malloc.h"
#include <stdio.h>

int main()
{
	void *ptr; //= malloc(4000);
	void *ptrs ;//= malloc(3900);
	/*show_alloc_mem();
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
	printf("---\n");*/
	ptr = malloc(4000);
	show_alloc_mem();
	printf("---\n");
	ptrs = malloc(19);
	show_alloc_mem();
	printf("---\n");
	free(ptrs);
	show_alloc_mem();
	printf("---\n");
	
	
	
	
	free((void *) 0x4242424242424242);
	return (0);
}