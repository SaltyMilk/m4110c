#include "malloc.h"

void *allocate(size_t size)
{
	void *ptr;

	if ((ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 0, 0)) == MAP_FAILED)
		return (NULL);
	ft_bzero(ptr, size);
	return (ptr);
}

void get_sizes(t_alloc_sizes *as)
{
	int pagesize = getpagesize();

	as->tiny_alloc = pagesize;
	as->tiny_limit = (pagesize / 100) - sizeof(t_heap_header);
	as->small_alloc = pagesize * 100;
	as->small_limit = pagesize - sizeof(t_heap_header);
}
