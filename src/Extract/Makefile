CC := gcc

###FLAGS
CFLAGS := -Wall -Wextra `pkg-config --cflags gtk+-3.0` -g
LDFLAGS := `pkg-config --libs gtk+-3.0` -lm

main: Detection.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) main Detection.o
.PHONY: clean
