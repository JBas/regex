CC=gcc
FLAGS=-Wall -Wextra -Werror -Wshadow -std=c99 -pedantic

TARGET=regex

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

stack:
	$(CC) $(CFLAGS) -o stack lib/stack.c main.c
