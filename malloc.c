#include "malloc.h"

void *search_free_zone(size_t size, char type)
{
	size_t i = 0;

		while (*(char *)(alloc_ptr + i))
		{
			if ((alloc_ptr + i)->type == type && (alloc_ptr + i)->available_space >= size)
				return (alloc_ptr + i)->ptr;
			i++;
		}
	return (NULL);
}

//dev note : free will bzero the memory this way we can easily find the next used (1) block.
void *search_free_slice(size_t size, char *start_ptr, size_t zone_size)
{
	size_t i = 0;

/*	while (i < zone_size)
	{
		//We found a previously used slice that suits our needs !
		if (((t_heap_header *) (start_ptr + i))->used == 0 && ((t_heap_header *) (start_ptr + i))->len >= size)
			return (start_ptr + i);

	}
*/
}


void *small_alloc(size_t size, t_alloc_sizes as)
{
	size_t	offset = 0;
	void	*free_zone_ptr;

	if (alloc_ptr) //Let's search for a free spot
	{
		if (!(free_zone_ptr = search_free_zone(size, 's')))
			;//create_new_zone(size)
		else
		{
			search_free_slice(size, free_zone_ptr, as.small_alloc);
		}
	}
	else if (!alloc_ptr && !(alloc_ptr = allocate(sizeof(t_alloc_zones + 1))))
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