CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -g
TARGET = proj2
SRC = proj2.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)