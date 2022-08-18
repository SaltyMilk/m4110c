#include "malloc.h"
#include <stdio.h>


int main()
{
	void *ptr;
	char *ptr2;
	size_t i;

	puts("Let's allocate two pointers");
	ptr = malloc(4002);
	ptr2 = malloc(19);
	show_alloc_mem();
	puts("---");
	puts("We'll fill ptr2 with 's'");
	for (i = 0; i < 19; i++)
		ptr2[i] = 's';
	ptr2[i] = 0;
	printf("content of ptr2 = %s\n\n", ptr2);

	puts("Now we'll realloc ptr2 to make it 1919 bytes instead of 19 bytes");
	ptr2 = realloc(ptr2, 1919);
	show_alloc_mem();
	printf("content of ptr2 = %s\n\n", ptr2);

	puts("Let's allocate a large block !\n");
	void *ptr3 = malloc(69420);
	show_alloc_mem();
	puts("\nAnd why not another tiny one !\n\n");
	void *ptr4 = malloc(7);
	puts("Actually that huge one is too big let's make it a bit smaller\n");
	ptr3 = realloc(ptr3, 42069);
	show_alloc_mem();

	puts("\nAt 42 I learned that I should always free memory !");
	free(ptr);
	free(ptr2);
	free(ptr3);
	free(ptr4);
	show_alloc_mem();
	return (0);
}