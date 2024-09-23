CC := gcc
CFLAGS := -Wall -Wextra #Only necessary flags
LDFLAGS := -lSDL2 -lSDL2_image #Linkers to SDL systems
TARGET = main

#All object files needed, categorized in directives:
SERVICES = SDL_Window_Manager.o Memory_Manager.o
SRC = Core_Manager.o $(addprefix Services/, $(SERVICES))

#Put all of them together
OBJS = main.o $(addprefix src/, $(SRC))

all: $(TARGET) #All command, the default command ran by writing "make"
$(TARGET): $(OBJS) #Creating all needed files to compile final program


#Clean command used to clean up any object and executable files
.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
