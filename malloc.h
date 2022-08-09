#ifndef MALLOC_H
#define MALLOC_H

#include "libft/libft.h"
#include <sys/mman.h>
#include <stdio.h>

typedef struct	s_alloc_sizes {
	size_t tiny_limit;
	size_t tiny_alloc;
	size_t small_limit;
	size_t small_alloc;
}				t_alloc_sizes;

typedef struct	s_heap_header {
	char	used;
	size_t	len;
}				t_heap_header;

typedef struct	s_alloc_zones {
	char					type; // 't' -> tiny, 's' -> small, 'l' -> large 
	size_t					available_space;
	void					*ptr;
}				t_alloc_zones;

//PUBLIC TOOLS
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void show_alloc_mem();

//INTERNAL TOOLS
void *allocate(size_t size);
void get_sizes(t_alloc_sizes *as);
t_alloc_zones *search_free_zone(size_t size, char type);
void *search_free_slice(size_t size, char *start_ptr, size_t zone_size);
size_t find_block_size(size_t index, size_t zone_size, char *ptr);
t_alloc_zones *create_new_zone(char type, t_alloc_sizes as, size_t size);
size_t alloc_zone_len();
size_t print_zone(char *ptr, size_t size);

#endif