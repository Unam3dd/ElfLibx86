NAME = elf_lib.a
SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)	
CFLAGS = -Wall -I includes

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< -ldl -m32


$(NAME) : $(OBJ_FILES)
	ar rc $(NAME) $^
	ranlib $(NAME)

clean: $(OBJ_FILES)
	rm $^

fclean: clean
	rm $(NAME)