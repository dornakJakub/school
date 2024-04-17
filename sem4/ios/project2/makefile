CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
TARGET=proj2

all: $(TARGET)

$(TARGET): proj2.c
	$(CC) $(CFLAGS) -o $(TARGET) proj2.c -lpthread -lrt

clean:
	rm -f $(TARGET)