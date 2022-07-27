#ifndef MALLOC_H
#define MALLOC_H

#include "libft/libft.h"
#include <sys/mman.h>

typedef struct	s_alloc_sizes {
	size_t tiny_limit;
	size_t tiny_alloc;
	size_t small_limit;
	size_t small_alloc;

}				t_alloc_sizes;

typedef struct	s_heap_header {
	size_t	len;
	char	used;

}				t_heap_header;


void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

#endif