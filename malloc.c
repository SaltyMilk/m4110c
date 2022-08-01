#include "malloc.h"

t_alloc_zones *allocs_ptr = NULL;

void *small_alloc(size_t size, t_alloc_sizes as)
{
	t_alloc_zones *free_zone_ptr;
	char	*free_slice_ptr;

	if (!allocs_ptr || !(free_zone_ptr = search_free_zone(size, 's')))
	{
		if (!(free_zone_ptr = create_new_zone('s', as, size)))
			return (NULL);
		free_slice_ptr = free_zone_ptr->ptr;
	}
	else
	{
		if (!(free_slice_ptr = search_free_slice(size, free_zone_ptr->ptr, as.small_alloc)))
			return (NULL);
	}
	((t_heap_header *)free_slice_ptr)->len = size;
	((t_heap_header *)free_slice_ptr)->used = (char) 1;
	free_zone_ptr->available_space -= size;
	return (free_slice_ptr);
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
		return small_alloc(size, as);
	else
	{
		//
	}
	return (NULL);
}
