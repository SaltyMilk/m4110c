#include "malloc.h"

extern t_alloc_zones *ft_allocs_ptr;

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

		while (*(char *)(ft_allocs_ptr + i))
		{
			if ((ft_allocs_ptr + i)->type == type && ( (ft_allocs_ptr + i)->available_space >= size + sizeof(t_heap_header) || !(ft_allocs_ptr +i)->ptr))
				return (ft_allocs_ptr + i);
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

	if (!ft_allocs_ptr)
		return 0;

	while (*(char *)(ft_allocs_ptr + i))
		i++;
	return i;
}

t_alloc_zones *create_new_zone(char type, t_alloc_sizes as, size_t size)
{
	void *ptr;
	size_t new_zones_size;
	if (!ft_allocs_ptr)
	{
		if (!(ft_allocs_ptr = allocate(sizeof(t_alloc_zones) + 1)))
			return (NULL);
		if (type == 's')
		{
			ft_allocs_ptr->available_space = as.small_alloc;
			if (!(ptr = allocate(as.small_alloc)))
				return (NULL);
		}
		else if (type == 't')
		{
			ft_allocs_ptr->available_space = as.tiny_alloc;
			if (!(ptr = allocate(as.tiny_alloc)))
				return (NULL);
		}
		else // 'l'
		{
			ft_allocs_ptr->available_space = size + sizeof(t_heap_header);
			if (!(ptr = allocate(size + sizeof(t_heap_header))))
				return (NULL);
		}
		//Common part
		ft_allocs_ptr->ptr = ptr;
		*(char *)(ft_allocs_ptr + 1) = '\0';
		ft_allocs_ptr->type = type;
		return (ft_allocs_ptr);
	}

	//We're appending a new zone
	new_zones_size = ((alloc_zone_len() + 1) * sizeof(t_alloc_zones)) + 1;
	if (!(ptr = allocate(new_zones_size)))
		return (NULL);
	ft_memcpy(ptr, ft_allocs_ptr, new_zones_size - sizeof(t_alloc_zones));
	if (munmap(ft_allocs_ptr, new_zones_size - sizeof(t_alloc_zones)) == -1)
		return (NULL);
	ft_allocs_ptr = ptr;
	new_zones_size = alloc_zone_len();
	if (type == 's')
	{
		(ft_allocs_ptr + new_zones_size)->available_space = as.small_alloc;
			if (!(ptr = allocate(as.small_alloc)))
				return (NULL);	
	}
	else if (type == 't')
	{
		(ft_allocs_ptr + new_zones_size)->available_space = as.tiny_alloc;
			if (!(ptr = allocate(as.tiny_alloc)))
				return (NULL);		
	}
	else // 'l'
	{
		(ft_allocs_ptr + new_zones_size)->available_space = size + sizeof(t_heap_header);
			if (!(ptr = allocate(size + sizeof(t_heap_header))))
				return (NULL);		
	}
	(ft_allocs_ptr + new_zones_size)->type = type;
	(ft_allocs_ptr + new_zones_size)->ptr = ptr;
	*(char *)(ft_allocs_ptr + (new_zones_size) + 1) = '\0';
	return ((ft_allocs_ptr + new_zones_size));
}
//mode = 0 no hexdump, 1 hexdumps blocks
size_t print_zone(char *ptr, size_t size, char mode)
{
	size_t i = 0;
	size_t ret = 0;
	if (!size)
		size = ((t_heap_header *)ptr)->len + sizeof(t_heap_header);
	while (i < size)
	{
		if (*(ptr + i))
		{
			ft_printf("0x%X - 0x%X : %u bytes\n", (ptr + i) + sizeof(t_heap_header), (ptr + i) + sizeof(t_heap_header) + ((t_heap_header *)(ptr + i))->len, ((t_heap_header *)(ptr + i))->len);
			if (mode)
				hexdump((ptr + i) + sizeof(t_heap_header), ((t_heap_header *)(ptr + i))->len);
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

//return 1 if true
int is_in_starr(size_t *arr, size_t size, size_t value)
{
	for (size_t i = 0; i < size; i++)
		if (arr[i] == value)
			return 1;
	return 0;
}

//Will modify indexs to have a list of index by sorted (small -> big) zone index
void sort_allocs(size_t *indexs, size_t n_zones)
{
	for (size_t i = 0; i < n_zones; i++)
		indexs[i] = (size_t)-1; //default value (case where ft_allocs_ptr->ptr == null)
	for (size_t i = 0; i < n_zones; i++)
	{
		void *min = (void *)-1;
		size_t min_index = -1;
		for (size_t j = 0; j < n_zones; j++)
		{
			if (ft_allocs_ptr[j].ptr && ft_allocs_ptr[j].ptr < min)
			{
				if (is_in_starr(indexs, n_zones, j))
					continue;
				min = ft_allocs_ptr[j].ptr;
				min_index = j;
			}
		}
		indexs[i] = min_index;
	}
}

t_alloc_zones *find_zone_by_ptr(void *ptr)
{
	size_t n_zones = alloc_zone_len();
	t_alloc_sizes as;

	get_sizes(&as);

	for (size_t i = 0; i < n_zones; i++)
	{
		if ((ft_allocs_ptr + i)->type == 's' && (ft_allocs_ptr + i)->ptr <= ptr && ptr < (ft_allocs_ptr + i)->ptr + as.small_alloc)
			return (ft_allocs_ptr + i);
		else if ((ft_allocs_ptr + i)->type == 't' && (ft_allocs_ptr + i)->ptr <= ptr && ptr < (ft_allocs_ptr + i)->ptr + as.tiny_alloc)
			return (ft_allocs_ptr + i);
		else if ((ft_allocs_ptr + i)->type == 'l' && (ft_allocs_ptr + i)->ptr && (ft_allocs_ptr + i)->ptr <= ptr && ptr < (ft_allocs_ptr + i)->ptr + ((t_heap_header *)(ft_allocs_ptr + i)->ptr)->len + sizeof(t_heap_header))
			return (ft_allocs_ptr + i);
	}
	

	return (NULL);	
}

//size is only usefull for large allocs
int allocate_ptr(char type, t_alloc_sizes as, t_alloc_zones *zone, size_t size)
{

	if (type == 's')
	{
		if (!(zone->ptr = allocate(as.small_alloc)))
			return 1;
		zone->available_space = as.small_alloc; // just for good measure but shouldn't be needed
	}
	else if (type == 't')
	{
		if (!(zone->ptr = allocate(as.tiny_alloc)))
			return 1;
		zone->available_space = as.tiny_alloc;
	}
	else // 'l'
	{
		if (!(zone->ptr = allocate(size + sizeof(t_heap_header))))
			return 1;
		zone->available_space = size + sizeof(t_heap_header); //this is mandatory !
	}
	return 0;
}

void hexdump(char *ptr, size_t size)
{
	ft_printf("Hexdump of block:");
	for (size_t i = 0; i < size; i++)
	{
		if (!(i % 16))
			ft_printf("\n");
		if (ft_isprint(ptr[i]) && !ft_isspace(ptr[i]))
			ft_printf(GREEN);
		else
			ft_printf(RED);
		ft_printf("%2X ", ptr[i]);
	}
		ft_printf(RESET);
	ft_printf("\nHuman readable:");
	for (size_t i = 0; i < size; i++)
	{
		if (!(i % 16))
			ft_printf("\n");
		if (ft_isprint(ptr[i]) && !ft_isspace(ptr[i]))
		{
			ft_printf(GREEN);
			ft_printf("%c  ", ptr[i]);
		}
		else
		{
			ft_printf(RED);
			ft_printf(".  ");
		}
	}
		ft_printf("\n\n");
		ft_printf(RESET);
}