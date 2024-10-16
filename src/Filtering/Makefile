# Build target
grayscale: grayscale.c filter
	gcc grayscale.c -o grayscale `pkg-config --cflags --libs gtk+-3.0` -lm

filter : filter.c
	gcc filter.c -o filter `pkg-config --cflags --libs gtk+-3.0` -lm
# Clean up
clean:
	rm -f grayscale filter
