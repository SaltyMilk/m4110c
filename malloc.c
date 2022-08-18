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
		if (!free_zone_ptr->ptr && allocate_ptr('s', as, free_zone_ptr, 19))
			return (NULL);
		if (!(free_slice_ptr = search_free_slice(size, free_zone_ptr->ptr, as.small_alloc)))
			return (NULL);
	}
	((t_heap_header *)free_slice_ptr)->len = size;
	((t_heap_header *)free_slice_ptr)->used = (char) 1;
	free_zone_ptr->available_space -= (size + sizeof(t_heap_header));
	return (free_slice_ptr + sizeof(t_heap_header));
}


void *tiny_alloc(size_t size, t_alloc_sizes as)
{
	t_alloc_zones *free_zone_ptr;
	char	*free_slice_ptr;
	if (!allocs_ptr || !(free_zone_ptr = search_free_zone(size, 't')))
	{
		if (!(free_zone_ptr = create_new_zone('t', as, size)))
			return (NULL);
		free_slice_ptr = free_zone_ptr->ptr;
	}
	else
	{
		if (!free_zone_ptr->ptr && allocate_ptr('t', as, free_zone_ptr, 19))
			return (NULL);
		if (!(free_slice_ptr = search_free_slice(size, free_zone_ptr->ptr, as.tiny_alloc)))
			return (NULL);
	}
	((t_heap_header *)free_slice_ptr)->len = size;
	((t_heap_header *)free_slice_ptr)->used = (char) 1;
	free_zone_ptr->available_space -= (size + sizeof(t_heap_header));
	return (free_slice_ptr + sizeof(t_heap_header));
}

void *large_alloc(size_t size, t_alloc_sizes as)
{
	t_alloc_zones *free_zone_ptr;
	if (!allocs_ptr || !(free_zone_ptr = search_free_zone(size, 'l')))
	{
		if (!(free_zone_ptr = create_new_zone('l', as, size)))
			return (NULL);
	}
	else
	{
		if (!free_zone_ptr->ptr && allocate_ptr('l', as, free_zone_ptr, size))
			return (NULL);
	}
	((t_heap_header *)(free_zone_ptr)->ptr)->len = size;
	((t_heap_header *)(free_zone_ptr)->ptr)->used = (char) 1;
	free_zone_ptr->available_space -= (size + sizeof(t_heap_header));
	return (free_zone_ptr->ptr + sizeof(t_heap_header));
}

void *malloc(size_t size)
{
	t_alloc_sizes as;

	get_sizes(&as);
	if (size <= as.tiny_limit)
		return tiny_alloc(size, as);	
	else if (size <= as.small_limit)
		return small_alloc(size, as);
	else
		return large_alloc(size, as);
	return (NULL);
}

void *realloc(void *ptr, size_t size)
{
	size_t block_size;
	t_alloc_sizes as;
	t_heap_header *ptrh = (t_heap_header *)(((char *)ptr) - sizeof(t_heap_header));
	void *tmp;
	void *ret = ptr;

	if (!ptr)
		return NULL;
	t_alloc_zones *zone = find_zone_by_ptr(ptr);
	if (!zone || !ptrh->used)//None allocated, or not used blocks will be ignored
		return NULL;
	get_sizes(&as);
	block_size = ptrh->len;
	if (!(tmp = allocate(block_size)))
		return NULL;
	ft_memcpy(tmp, ptr, block_size);//save content of previous allocation
	//if we don't need a zone transfer
	if (((zone->type == 't' && size <= as.tiny_limit) || (zone->type == 's' && size <= as.small_limit && size > as.tiny_limit)))
	{
		if (zone->type == 't')
		{
			if (ptr + size < zone->ptr + as.tiny_alloc)//we can just increase block size
			{
				zone->available_space += ptrh->len;
				zone->available_space -= size;
				ptrh->len = size;
			}
			else
			{
				free(ptr);
				if (!(ret = malloc(size)))
					return NULL;
				if (size >= block_size)
					ft_memcpy(ret, tmp, block_size);
				else
					ft_memcpy(ret, tmp, size);
			}
		}
		else if (zone->type == 's')
		{
			if (ptr + size < zone->ptr + as.small_alloc)
			{
				zone->available_space += ptrh->len;
				zone->available_space -= size;
				ptrh->len = size;
			}
			else
			{
				free(ptr);
				if (!(ret = malloc(size)))
					return NULL;
				if (size >= block_size)
					ft_memcpy(ret, tmp, block_size);
				else
					ft_memcpy(ret, tmp, size);
			}	
		}
	}
	else //zone transfer
	{
		free(ptr);
		if (!(ret = malloc(size)))
			return NULL;
		if (size >= block_size)
			ft_memcpy(ret, tmp, block_size);
		else
			ft_memcpy(ret, tmp, size);
	}
	munmap(tmp, block_size);
	return ret;
}

void free(void *ptr)
{
	size_t block_size;
	t_alloc_sizes as;
	t_heap_header *ptrh = (t_heap_header *)(((char *)ptr) - sizeof(t_heap_header));

	if (!ptr)
		return;
	t_alloc_zones *zone = find_zone_by_ptr(ptr);
	if (!zone || !ptrh->used)//None allocated, or not used blocks will be ignored
		return;
	block_size = ptrh->len;
	zone->available_space += block_size + sizeof(t_heap_header);
	ft_bzero(ptrh, block_size + sizeof(t_heap_header));
	get_sizes(&as);
	if ((zone->type == 't' && zone->available_space == as.tiny_alloc)  ||
		(zone->type == 's' && zone->available_space == as.small_alloc) ||
		zone->type == 'l')
	{
		munmap(zone->ptr, zone->available_space);
		zone->ptr = NULL;
	}
}

void show_alloc_mem()
{
	t_alloc_sizes as;
	size_t n_zones = alloc_zone_len();
	size_t total = 0;

	get_sizes(&as);
	size_t	indexs[n_zones];

	sort_allocs(indexs, n_zones);
	for (size_t i = 0; i < n_zones && indexs[i] != (size_t)-1; i++)
	{
		if (allocs_ptr[indexs[i]].type == 't')
			ft_printf("TINY : ");
		else if (allocs_ptr[indexs[i]].type == 's')
			ft_printf("SMALL : ");
		else if (allocs_ptr[indexs[i]].type == 'l')
			ft_printf("LARGE : ");
		ft_printf("Ox%X\n", (unsigned long long)allocs_ptr[indexs[i]].ptr);
		//Now we gotta parse the zone to display each block
		if (allocs_ptr[indexs[i]].type == 's')
			total += print_zone(allocs_ptr[indexs[i]].ptr, as.small_alloc);
		else if (allocs_ptr[indexs[i]].type == 't')
			total += print_zone(allocs_ptr[indexs[i]].ptr, as.tiny_alloc);
		else if (allocs_ptr[indexs[i]].type == 'l')
			total += print_zone(allocs_ptr[indexs[i]].ptr, 0);
	}

	ft_printf("Total : %u bytes\n", total);	
}
