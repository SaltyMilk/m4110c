#include "malloc.h"

t_alloc_zones *allocs_ptr = NULL;

void *small_alloc(size_t size, t_alloc_sizes as)
{
	void	*free_zone_ptr;
	void	*free_slice_ptr;

	if (allocs_ptr) //Let's search for a free spot
	{
		if (!(free_zone_ptr = search_free_zone(size, 's')))
			;//create_new_zone(size)
		else
		{
			if ((free_slice_ptr = search_free_slice(size, free_zone_ptr, as.small_alloc)))
				return (free_slice_ptr);
			return (NULL);
		}
	}
	else
	{
		if (!(allocs_ptr = allocate(sizeof(t_alloc_zones) + 1)))
			return (NULL);
	}
	return (NULL);
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