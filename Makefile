MSRC = malloc.c utils.c
CC = gcc
CFLAGS = -Wall -Wextra -Werror
MOBJ = $(MSRC:.c=.o)

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif


NAME = libft_malloc_$(HOSTTYPE).so 

all: libft.a $(NAME)

$(NAME): $(MOBJ)
	gcc $(CFLAGS) libft.a $? -shared -o $(NAME)
	ln -s $(NAME) libft_malloc.so

%.o: %.c
	$(CC) -c $(CFLAGS) $?

libft.a:
	make -C libft
	cp ./libft/libft.a libft.a

clean: 
	rm -f $(MOBJ)
	make -C libft clean
fclean:
	rm -f $(MOBJ) $(NAME) libft_malloc.so testx
#libft.a
	make -C libft fclean

test:
	gcc test.c $(NAME) -o testx; ./testx

re : fclean all

.PHONY : clean fclean re  all