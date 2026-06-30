CC      = gcc
CFLAGS  = $(shell cat compile_flags.txt)
LDFLAGS = -lcrypto -lz

ifeq ($(shell uname), Darwin)
	CFLAGS  += -I$(shell brew --prefix openssl)/include
	LDFLAGS += -L$(shell brew --prefix openssl)/lib
endif

TARGET  = minigit

BUILD   = build

SRC     = $(shell find src -name "*.c")

OBJ     = $(patsubst %.c,$(BUILD)/%.o,$(SRC))

TEST_DIR    = tests
TEST_TARGET = minigit-tests
TEST_SRC    = $(shell find $(TEST_DIR) -name "*.c")
TEST_OBJ    = $(patsubst %.c,$(BUILD)/%.o,$(TEST_SRC))

.PHONY: all clean tests test porcelain plumbing utils

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/src/%.o: src/%.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD) $(TARGET) $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(filter-out $(BUILD)/src/cli/main.o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_DIR) -c $< -o $@

tests: $(TEST_TARGET)
	@./$(TEST_TARGET) $(addprefix /,$(filter-out tests,$(MAKECMDGOALS)))

test: $(TEST_TARGET)
	@./$(TEST_TARGET) $(if $(SUITE),$(SUITE))

porcelain plumbing utils:
	@:
