# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude   # -I adds include/ to the header search path

# Name of the final executable
TARGET  = minigit

# Directory where object files will be placed
BUILD   = build

# Source files: main.c plus everything under src/
SRC     = main.c $(wildcard src/*.c)

# Derive object file list: strip directory, swap .c for .o, prefix with build/
# e.g. src/foo.c -> build/foo.o, main.c -> build/main.o
OBJ     = $(addprefix $(BUILD)/, $(notdir $(SRC:.c=.o)))

# Phony targets are not real files — always run when requested
.PHONY: all clean

# Default target: build the executable
all: $(TARGET)

# Link all object files into the final binary
# $@ = target name (minigit), $^ = all prerequisites (the .o files)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile root-level .c files (e.g. main.c) into build/
# | $(BUILD) is an order-only prerequisite: ensures the dir exists before compiling
$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/*.c files into build/
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)

# Remove all build artifacts
clean:
	rm -rf $(BUILD) $(TARGET)
