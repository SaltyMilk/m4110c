#include "malloc.h"

void get_sizes(t_alloc_sizes *as)
{
	int pagesize = getpagesize();

	as->tiny_alloc = pagesize;
	as->tiny_limit = (pagesize / 100) - sizeof(t_heap_header);
	as->small_alloc = pagesize * 100;
	as->small_limit = pagesize - sizeof(t_heap_header);
}

void *malloc(size_t size)
{
	t_alloc_sizes as;

	get_sizes(&as);

	if (size <= as.tiny_limit)
	{
		//
	}
	else if (size <= as.small_limit)
	{
		//
	}
	else
	{
		//
	}
	return (NULL);
}