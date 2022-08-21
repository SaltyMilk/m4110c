#include "malloc.h"

t_alloc_zones				*ft_allocs_ptr	= NULL;
static pthread_mutex_t		ft_mutex		= PTHREAD_MUTEX_INITIALIZER;


void *small_alloc(size_t size, t_alloc_sizes as)
{
	t_alloc_zones *free_zone_ptr;
	char	*free_slice_ptr;
	if (!ft_allocs_ptr || !(free_zone_ptr = search_free_zone(size, 's')))
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
	if (!ft_allocs_ptr || !(free_zone_ptr = search_free_zone(size, 't')))
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
	if (!ft_allocs_ptr || !(free_zone_ptr = search_free_zone(size, 'l')))
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

void *calloc(size_t n, size_t s)
{
	ft_printf("My calloc was used\n");
	return malloc(n * s);
}

void *malloc(size_t size)
{
	t_alloc_sizes as;
	void *ret;

	if (size >= SIZE_MAX - sizeof(t_heap_header))
		return NULL;
	ft_printf("My malloc was used !\n");
	pthread_mutex_lock(&ft_mutex);
	get_sizes(&as);
	if (size <= as.tiny_limit)
	{
		ret = tiny_alloc(size, as);	
		pthread_mutex_unlock(&ft_mutex);
		if (ret)
			ft_printf("suceess[0x%X]\n", ret);
		return ret;
	}
	else if (size <= as.small_limit)
	{
		ret = small_alloc(size, as);
		pthread_mutex_unlock(&ft_mutex);
		if (ret)
			ft_printf("suceess[0x%X]\n", ret);
		return ret;
	}
	else
	{
		ret = large_alloc(size, as);
		pthread_mutex_unlock(&ft_mutex);
		if (ret)
			ft_printf("suceess[0x%X]\n", ret);
		return ret;
	}
}

void *realloc(void *ptr, size_t size)
{
	size_t block_size;
	t_alloc_sizes as;
	t_heap_header *ptrh = (t_heap_header *)(((char *)ptr) - sizeof(t_heap_header));
	void *tmp;
	void *ret = ptr;

	ft_printf("My realloc was used !\n");
	if (!ptr || size >= SIZE_MAX - sizeof(t_heap_header))
		return NULL;
	pthread_mutex_lock(&ft_mutex);
	t_alloc_zones *zone = find_zone_by_ptr(ptr);
	if (!zone || !ptrh->used)//None allocated, or not used blocks will be ignored
	{
		pthread_mutex_unlock(&ft_mutex);
		return NULL;
	}
	get_sizes(&as);
	block_size = ptrh->len;
	if (!(tmp = allocate(block_size)))
	{
		pthread_mutex_unlock(&ft_mutex);
		return NULL;
	}
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
				pthread_mutex_unlock(&ft_mutex);
				free(ptr);
				if (!(ret = malloc(size)))
					return NULL;
				pthread_mutex_lock(&ft_mutex);
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
				pthread_mutex_unlock(&ft_mutex);
				free(ptr);
				if (!(ret = malloc(size)))
					return NULL;
				pthread_mutex_lock(&ft_mutex);
				if (size >= block_size)
					ft_memcpy(ret, tmp, block_size);
				else
					ft_memcpy(ret, tmp, size);
			}	
		}
	}
	else //zone transfer
	{
		pthread_mutex_unlock(&ft_mutex);
		free(ptr);
		if (!(ret = malloc(size)))
			return NULL;
		pthread_mutex_lock(&ft_mutex);
		if (size >= block_size)
			ft_memcpy(ret, tmp, block_size);
		else
			ft_memcpy(ret, tmp, size);
	}
	munmap(tmp, block_size);
	pthread_mutex_unlock(&ft_mutex);
	ft_printf("My realloc was used and we got to the end of it ! !\n");
	return ret;
}

void free(void *ptr)
{
	size_t block_size;
	t_alloc_sizes as;
	t_heap_header *ptrh = (t_heap_header *)(((char *)ptr) - sizeof(t_heap_header));

	ft_printf("My free was used !");
	ft_printf(" on [0x%X]\n", ptr);
	if (!ptr)
		return;
	pthread_mutex_lock(&ft_mutex);
	t_alloc_zones *zone = find_zone_by_ptr(ptr);
	if (!zone || !ptrh->used)//None allocated, or not used blocks will be ignored
	{
		pthread_mutex_unlock(&ft_mutex);
		return;
	}
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
		delete_meta_zone(zone);
	}
	pthread_mutex_unlock(&ft_mutex);
	ft_printf("Free success !\n");
}

void show_alloc_mem()
{
	pthread_mutex_lock(&ft_mutex);
	t_alloc_sizes as;
	size_t n_zones = alloc_zone_len();
	size_t total = 0;

	get_sizes(&as);
	size_t	indexs[n_zones];

	sort_allocs(indexs, n_zones);
	for (size_t i = 0; i < n_zones && indexs[i] != (size_t)-1; i++)
	{
		if (ft_allocs_ptr[indexs[i]].type == 't')
			ft_printf("TINY : ");
		else if (ft_allocs_ptr[indexs[i]].type == 's')
			ft_printf("SMALL : ");
		else if (ft_allocs_ptr[indexs[i]].type == 'l')
			ft_printf("LARGE : ");
		ft_printf("Ox%X\n", (unsigned long long)ft_allocs_ptr[indexs[i]].ptr);
		//Now we gotta parse the zone to display each block
		if (ft_allocs_ptr[indexs[i]].type == 's')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, as.small_alloc, 0);
		else if (ft_allocs_ptr[indexs[i]].type == 't')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, as.tiny_alloc, 0);
		else if (ft_allocs_ptr[indexs[i]].type == 'l')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, 0, 0);
	}

	ft_printf("Total : %u bytes\n", total);	
	pthread_mutex_unlock(&ft_mutex);
}

void show_alloc_mem_ex()
{
	pthread_mutex_lock(&ft_mutex);
	t_alloc_sizes as;
	size_t n_zones = alloc_zone_len();
	size_t total = 0;

	get_sizes(&as);
	size_t	indexs[n_zones];

	sort_allocs(indexs, n_zones);
	for (size_t i = 0; i < n_zones && indexs[i] != (size_t)-1; i++)
	{
		if (ft_allocs_ptr[indexs[i]].type == 't')
			ft_printf("TINY : ");
		else if (ft_allocs_ptr[indexs[i]].type == 's')
			ft_printf("SMALL : ");
		else if (ft_allocs_ptr[indexs[i]].type == 'l')
			ft_printf("LARGE : ");
		ft_printf("Ox%X\n", (unsigned long long)ft_allocs_ptr[indexs[i]].ptr);
		//Now we gotta parse the zone to display each block
		if (ft_allocs_ptr[indexs[i]].type == 's')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, as.small_alloc, 1);
		else if (ft_allocs_ptr[indexs[i]].type == 't')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, as.tiny_alloc, 1);
		else if (ft_allocs_ptr[indexs[i]].type == 'l')
			total += print_zone(ft_allocs_ptr[indexs[i]].ptr, 0, 1);
	}

	ft_printf("Total : %u bytes\n", total);	
	pthread_mutex_unlock(&ft_mutex);

}