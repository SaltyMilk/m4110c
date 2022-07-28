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

t_alloc_zones *create_new_zone(char type, t_alloc_sizes as, size_t size)
{
	void *ptr;

	if (!allocs_ptr)
	{
		if (!(allocs_ptr = allocate(sizeof(t_alloc_zones) + 1)))
			return (NULL);
		if (type == 's')
		{
			allocs_ptr->available_space = as.small_alloc - (size + sizeof(t_heap_header));
			if (!(ptr = allocate(as.small_alloc)))
				return (NULL);
		}
		else if (type == 't')
		{
			allocs_ptr->available_space = as.tiny_alloc - (size + sizeof(t_heap_header));
			if (!(ptr = allocate(as.tiny_alloc)))
				return (NULL);
		}
		else if (type == 'l')
		{
			allocs_ptr->available_space = 0;
			if (!(ptr = allocate(size)))
				return (NULL);
		}
		//Common part
		allocs_ptr->ptr = ptr;
		*(char *)(allocs_ptr + 1) = '\0';
		allocs_ptr->type = type;
		return (allocs_ptr);
	}
	//Continue here
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