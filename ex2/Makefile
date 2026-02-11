# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

# Folders
SRC_DIR = src
BIN_DIR = bin
UNITY_DIR = lib/unity

# Sources
MAIN_SRCS = $(SRC_DIR)/main_ex2.c $(SRC_DIR)/edit_distance.c
TEST_SRCS = $(SRC_DIR)/test_ex2.c $(SRC_DIR)/edit_distance.c $(UNITY_DIR)/unity.c

# Targets
MAIN_EXE = $(BIN_DIR)/main_ex2
TEST_EXE = $(BIN_DIR)/test_ex2

.PHONY: all clean

# Default target
all: $(MAIN_EXE) $(TEST_EXE)

# Main program
$(MAIN_EXE): $(MAIN_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Test executable
$(TEST_EXE): $(TEST_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Clean build artifacts
clean:
	rm -f $(SRC_DIR)/*.o
	rm -f $(BIN_DIR)/main_ex2 $(BIN_DIR)/test_ex3
