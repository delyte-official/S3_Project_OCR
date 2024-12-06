CC := gcc

###FLAGS
CFLAGS := -Wall -Wextra `pkg-config --cflags gtk+-3.0` -rdynamic
LDFLAGS := `pkg-config --libs gtk+-3.0` -lm


#All object files needed, categorized in directives:
INTERFACE = GTK_Window.o Events.o Interface.o
FILTERING = Filter.o
EXTRACTION = Extract.o Extraction_Manager.o
OCR = OCR_Manager.o
SOLVING = Solver_Manager.o
SRC = Core_Manager.o \
	$(addprefix Interface/, $(INTERFACE)) \
	$(addprefix Filter/, $(FILTERING)) \
	$(addprefix Extract/, $(EXTRACTION)) \
	$(addprefix OCR/, $(OCR))
#	$(addprefix Solving/, $(SOLVING)) #Debug.o

#SOLVER = src/Solving/solver #Independent program, so independant category
CUT_IMAGES = src/bin/grid* src/bin/word*

#Put all of them together
OBJS = main.o \
	$(addprefix src/, $(SRC))

TARGET = main
all: $(TARGET) #All command, the default command ran by writing "make"
$(TARGET): $(OBJS) #Creating all needed files to compile final program
	#$(CC) -o $(SOLVER) $(addsuffix .c, $(SOLVER))
	#$(CC) -o $(OCR) $(addsuffix .c, $(OCR)) -lm
	#$(CC) $(CFLAGS) -o $(EXTRACT) $(addsuffix .c, $(EXTRACT)) $(LDFLAGS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Clean command used to clean up any object and executable files
clean:
	$(RM) $(OBJS) $(TARGET) $(SOLVER) nohup.out $(CUT_IMAGES)

#Reset command to clean, clear and make
remake:
	make clean
	clear
	make

# PHONY Targets
.PHONY: all make reset
