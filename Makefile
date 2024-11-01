CC := gcc

###FLAGS
CFLAGS := -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS := `pkg-config --libs gtk+-3.0` -lm


#All object files needed, categorized in directives:
INTERFACE = Events.o GTK_Window.o Interface_Manager.o
FILTERING = Prefilter.o
SOLVING = Solver_Manager.o
SOLVER = src/Solving/solver #Independent compiling, so independant category
SRC = Core_Manager.o \
	$(addprefix Interface/, $(INTERFACE)) \
	$(addprefix Solving/, $(SOLVING)) \
	$(addprefix Filtering/, $(FILTERING)) #Debug.o

#Put all of them together
OBJS = main.o \
	$(addprefix src/, $(SRC))

TARGET = main
all: $(TARGET) #All command, the default command ran by writing "make"
$(TARGET): $(OBJS) #Creating all needed files to compile final program
	$(CC) -o $(SOLVER) $(addsuffix .c, $(SOLVER))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Clean command used to clean up any object and executable files
clean:
	$(RM) $(OBJS) $(TARGET) $(SOLVER)

#Reset command to clean, clear and make
remake:
	make clean
	clear
	make

# PHONY Targets
.PHONY: all make reset
