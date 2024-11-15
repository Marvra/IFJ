# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall -Wextra

# Source files
SOURCES = main.c lexer.c token.c stack.c parser.c ast.c symtable.c semantic.c expr.c

# Object files (replace .c with .o)
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = program

# Default target to build the program
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean up the build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Make all targets phony (not files)
.PHONY: all clean