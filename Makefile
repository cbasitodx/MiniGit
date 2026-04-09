# Compiler and flags
CC      = gcc
CFLAGS  = $(shell cat compile_flags.txt)
LDFLAGS = -lcrypto

# macOS: OpenSSL is keg-only, so the headers and libs aren't on the default path
ifeq ($(shell uname), Darwin)
	CFLAGS  += -I$(shell brew --prefix openssl)/include
	LDFLAGS += -L$(shell brew --prefix openssl)/lib
endif

# Name of the final executable
TARGET  = minigit

# Directory where object files will be placed
BUILD   = build

# Source files: main.c plus everything under src/
SRC     = main.c $(shell find src -name "*.c")

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
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile root-level .c files (e.g. main.c) into build/
# | $(BUILD) is an order-only prerequisite: ensures the dir exists before compiling
$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/*.c files into build/
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/plumbing/*.c files into build/
$(BUILD)/%.o: src/plumbing/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/porcelain/*.c files into build/
$(BUILD)/%.o: src/porcelain/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/utils/*.c files into build/
$(BUILD)/%.o: src/utils/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)

# Remove all build artifacts
clean:
	rm -rf $(BUILD) $(TARGET)
