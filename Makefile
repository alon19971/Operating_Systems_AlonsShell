# Define the compiler to be used
CC=gcc

# Compiler flags:
# -Wall: Enable all warnings
# -g: Produce debugging information in the operating system's native format
# -Iinclude: Add the 'include' directory to the list of directories to be searched for header files
CFLAGS=-Wall -g -Iinclude

# Linker flags, empty in this case but can be used for linking against libraries
LDFLAGS=

# Source files involved in the build
SOURCES=src/episodeA.c src/main.c src/episodeB.c src/episodeC.c src/episodeD.c

# Object files corresponding to source files
# This automatic variable substitution replaces '.c' extensions with '.o'
OBJECTS=$(SOURCES:.c=.o)

# Name of the final executable to be produced
EXECUTABLE=AlonsShell

# The default target: build the executable and then run it
all: $(EXECUTABLE)
	./$(EXECUTABLE)

# Rule to link the object files into the final executable
# Depends on the OBJECTS variable, and uses the compiler set in CC to link them
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Pattern rule for object file generation
# For each .c file, compile it into an .o file using the compiler flags defined in CFLAGS
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target: remove object files and the executable to start from scratch
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Mark 'clean' and 'all' as phony targets
# Phony targets are not files; they are just names for actions to be taken by make
.PHONY: clean all
