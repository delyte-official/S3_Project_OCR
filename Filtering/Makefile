# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2 -lSDL2_image -lm
TARGET = grayscale
SRC = grayscale.c

# Build target
$(TARGET): $(SRC) filter
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

filter : filter.c
	gcc filter.c -o filter `pkg-config --cflags --libs gtk+-3.0` -lm
# Clean up
clean:
	rm -f $(TARGET) filter
