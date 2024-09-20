CC := gcc
CFLAGS := -Wall -Wextra #Only necessary flags
LDFLAGS := -lSDL2 -lSDL2_image #Linkers to SDL systems
TARGET = main
OBJS = main.o

all: $(TARGET) #All command, the default command ran by writing "make"
$(TARGET): $(OBJS) #Creating all needed files to compile final program

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
