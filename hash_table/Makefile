CC=gcc
CFLAGS=-std=c11

NAME=hash_table

.PHONY: all clean

all: $(NAME)

$(NAME): $(NAME).o
	$(CC) $(CFLAGS) $^ -o $@
