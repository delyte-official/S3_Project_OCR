CC := gcc

###FLAGS
CFLAGS := -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS := -lSDL2 -lSDL2_image `pkg-config --libs gtk+-3.0` -lm


#All object files needed, categorized in directives:
INTERFACE = Events_Manager.o Window_Manager.o Interface_Manager.o
EXTRACTION = extraction.o
SRC = Core_Manager.o Debug.o \
	$(addprefix Interface/, $(INTERFACE)) \
	$(addprefix Extraction/, $(EXTRACTION))

#Put all of them together
OBJS = main.o \
	$(addprefix src/, $(SRC))

TARGET = main
all: $(TARGET) #All command, the default command ran by writing "make"
$(TARGET): $(OBJS) #Creating all needed files to compile final program
	$(CC) -o src/solver src/solver.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Clean command used to clean up any object and executable files
.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET) src/solver
