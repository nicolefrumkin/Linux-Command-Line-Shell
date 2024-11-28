# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = hw1shell

# Source files
SRCS = hw1shell.c
OBJS = hw1shell.o

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile the source file into an object file
hw1shell.o: hw1shell.c hw1shell.h
	$(CC) $(CFLAGS) -c hw1shell.c

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)
