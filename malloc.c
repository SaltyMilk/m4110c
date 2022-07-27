#include "malloc.h"

void *search_free_zone(size_t size, char type)
{
	size_t i = 0;

		while (*(char *)(allocs_ptr + i))
		{
			if ((allocs_ptr + i)->type == type && (allocs_ptr + i)->available_space >= size)
				return (allocs_ptr + i)->ptr;
			i++;
		}
	return (NULL);
}

size_t find_block_size(size_t index, size_t zone_size, char *ptr)
{
	size_t start = index;

	while (!ptr[index] && index < zone_size)
		index++;
	return index - start;
}

//dev note : free will bzero the memory this way we can easily find the next used (1) block.
void *search_free_slice(size_t size, char *start_ptr, size_t zone_size)
{
	size_t i = 0;

	while (i < zone_size)
	{
		if (((t_heap_header *)(start_ptr + i))->used)
			i += ((t_heap_header *)(start_ptr + i))->len + sizeof(t_heap_header); //skip used block
		else if (find_block_size(i, zone_size, start_ptr) >= size) //We found a block big enough.
			return (start_ptr + i);
		else //Free but too smoll block
			i += find_block_size(i, zone_size, start_ptr);
	}
	return (NULL);
}


void *small_alloc(size_t size, t_alloc_sizes as)
{
	void	*free_zone_ptr;

	if (allocs_ptr) //Let's search for a free spot
	{
		if (!(free_zone_ptr = search_free_zone(size, 's')))
			;//create_new_zone(size)
		else
		{
			search_free_slice(size, free_zone_ptr, as.small_alloc);
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