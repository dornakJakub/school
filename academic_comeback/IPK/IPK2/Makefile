CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

TARGET = ipk-rdt
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

# build
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	rm -f $(OBJ) $(TARGET)

# nix
NixDevShellName:
	@echo c

.PHONY: all clean NixDevShellName