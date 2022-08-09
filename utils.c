#include "malloc.h"

extern t_alloc_zones *allocs_ptr;

void *allocate(size_t size)
{
	void *ptr;
	if ((ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0)) == MAP_FAILED)
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

t_alloc_zones *search_free_zone(size_t size, char type)
{
	size_t i = 0;

		while (*(char *)(allocs_ptr + i))
		{
			if ((allocs_ptr + i)->type == type && (allocs_ptr + i)->available_space >= size)
				return (allocs_ptr + i);
			i++;
		}
	return (NULL);
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

size_t find_block_size(size_t index, size_t zone_size, char *ptr)
{
	size_t start = index;

	while (index < zone_size && !ptr[index])
		index++;
	return index - start;
}

size_t alloc_zone_len()
{
	size_t i = 0;

	if (!allocs_ptr)
		return 0;

	while (*(char *)(allocs_ptr + i))
		i++;
	return i;
}

t_alloc_zones *create_new_zone(char type, t_alloc_sizes as, size_t size)
{
	void *ptr;
	size_t new_zones_size;
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
		else // 'l'
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

	//We're appending a new zone
	new_zones_size = ((alloc_zone_len() + 1) * sizeof(t_alloc_zones)) + 1;
	if (!(ptr = allocate(new_zones_size)))
		return (NULL);
	ft_memcpy(ptr, allocs_ptr, new_zones_size - sizeof(t_alloc_zones));
	if (munmap(allocs_ptr, new_zones_size - sizeof(t_alloc_zones)) == -1)
		return (NULL);
	new_zones_size = alloc_zone_len();
	allocs_ptr = ptr;
	if (type == 's')
	{
		(allocs_ptr + new_zones_size)->available_space = as.small_alloc - (size + sizeof(t_heap_header));
			if (!(ptr = allocate(as.small_alloc)))
				return (NULL);	
	}
	else if (type == 't')
	{
		(allocs_ptr + new_zones_size)->available_space = as.tiny_alloc - (size + sizeof(t_heap_header));
			if (!(ptr = allocate(as.tiny_alloc)))
				return (NULL);		
	}
	else // 'l'
	{
		(allocs_ptr + new_zones_size)->available_space = 0;
			if (!(ptr = allocate(size)))
				return (NULL);		
	}
	(allocs_ptr + new_zones_size)->type = type;
	(allocs_ptr + new_zones_size)->ptr = ptr;
	*(char *)(allocs_ptr + (new_zones_size) + 1) = '\0';
	return ((allocs_ptr + new_zones_size));
}

size_t print_zone(char *ptr, size_t size)
{
	size_t i = 0;
	size_t ret = 0;

	while (i < size)
	{
		if (*(ptr + i))
		{
			ft_printf("0x%X - 0x%X : %u bytes\n", (ptr + i) + sizeof(t_heap_header), (ptr + i) + sizeof(t_heap_header) + ((t_heap_header *)(ptr + i))->len, ((t_heap_header *)(ptr + i))->len);
			ret += ((t_heap_header *)(ptr + i))->len;
			i += ((t_heap_header *)(ptr + i))->len + sizeof(t_heap_header);
		}
		else
		{
			while (i < size && !*(ptr + i))
				i++;
		}
	}
	return ret;
}