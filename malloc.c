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
	puts("---debug--");
	printf("slimit=%lu\n", as.small_limit);
	printf("salloc=%lu\n", as.small_alloc);
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

//Will modify indexs to have a list of index by sorted (small -> big) zone index
void sort_allocs(size_t *indexs, size_t n_zones)
{
	for (size_t i = 0; i < n_zones; i++)
		indexs[i] = (size_t)-1; //default value (case where allocs_ptr->ptr == null)
	for (size_t i = 0; i < n_zones; i++)
	{
		void *min = (void *)-1;
		size_t min_index = 0;
		for (size_t j = 0; j < n_zones; j++)
		{
			if (allocs_ptr[j].ptr && allocs_ptr[j].ptr < min)
			{
				min = allocs_ptr[j].ptr;
				min_index = j;
			}
		}
		indexs[i] = min_index;
	}
}

void show_alloc_mem()
{
	t_alloc_sizes as;
	size_t n_zones = alloc_zone_len();
	
	get_sizes(&as);
	size_t	indexs[n_zones];

	sort_allocs(indexs, n_zones);

	for (size_t i = 0; i < n_zones && indexs[i] != (size_t)-1; i++)
	{
		if (allocs_ptr[indexs[i]].type == 't')
			ft_printf("TINY : ");
		else if (allocs_ptr[indexs[i]].type == 's')
			ft_printf("SMALL : ");
		printf("Ox%llX\n", (unsigned long long)allocs_ptr[indexs[i]].ptr);
		//Now we gotta parse the zone to display each block
	}	
}
