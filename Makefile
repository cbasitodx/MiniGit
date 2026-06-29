# Compiler and flags
CC      = gcc
CFLAGS  = $(shell cat compile_flags.txt)
LDFLAGS = -lcrypto -lz

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
SRC     = main.c $(shell find src -name "*.c" ! -path "$(TEST_DIR)/*")

# Derive object file list: strip directory, swap .c for .o, prefix with build/
# e.g. src/foo.c -> build/foo.o, main.c -> build/main.o
OBJ     = $(addprefix $(BUILD)/, $(notdir $(SRC:.c=.o)))

# Test binary
TEST_DIR    = src/test
TEST_TARGET = minigit-tests
TEST_SRC    = $(TEST_DIR)/test_main.c \
              $(TEST_DIR)/fixtures.c \
              $(TEST_DIR)/munit/munit.c \
              $(TEST_DIR)/porcelain/test_init.c \
              $(TEST_DIR)/plumbing/test_hash_content.c \
              $(TEST_DIR)/plumbing/test_rev_parse.c \
              $(TEST_DIR)/plumbing/test_cat_file.c \
              $(TEST_DIR)/utils/test_errors.c \
              $(TEST_DIR)/utils/test_read.c
TEST_OBJ    = $(addprefix $(BUILD)/, $(notdir $(TEST_SRC:.c=.o)))

# Phony targets are not real files — always run when requested
.PHONY: all clean tests test porcelain plumbing utils

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
	rm -rf $(BUILD) $(TARGET) $(TEST_TARGET)

# ── Test targets ──────────────────────────────────────────────────────────────
#
# make tests              — run all tests
# make tests porcelain    — run only porcelain tests
# make tests plumbing     — run only plumbing tests
# make tests utils        — run only utils tests
# make test SUITE=<path>  — run a specific suite, e.g. SUITE=/porcelain/init
#
# Test names follow munit's path convention: /porcelain/init/creates_dir
# You can also run the binary directly:
#   ./minigit-tests /plumbing/hash_content
#

$(TEST_TARGET): $(TEST_OBJ) $(filter-out $(BUILD)/main.o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile test source files
$(BUILD)/%.o: $(TEST_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

$(BUILD)/%.o: $(TEST_DIR)/munit/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

$(BUILD)/%.o: $(TEST_DIR)/porcelain/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

$(BUILD)/%.o: $(TEST_DIR)/plumbing/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

$(BUILD)/%.o: $(TEST_DIR)/utils/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

tests: $(TEST_TARGET)
	@./$(TEST_TARGET) $(addprefix /,$(filter-out tests,$(MAKECMDGOALS)))

test: $(TEST_TARGET)
	@./$(TEST_TARGET) $(if $(SUITE),$(SUITE))

# Absorb bundle names as no-op targets so make tests <bundle> works without errors.
porcelain plumbing utils:
	@:
