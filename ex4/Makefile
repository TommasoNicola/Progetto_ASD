# Cartelle
SRC_DIR=src
INC_DIR=include
LIB_DIR=lib/unity
BIN_DIR=ex3-4/bin

# File sorgenti
SRCS=$(SRC_DIR)/graph.c $(SRC_DIR)/hash_table.c
MAIN=$(SRC_DIR)/main_ex4.c
TEST=$(SRC_DIR)/test_ex4.c
UNITY=$(LIB_DIR)/unity.c

# Output
MAIN_EXEC=$(BIN_DIR)/main_ex3-4
TEST_EXEC=$(BIN_DIR)/test_ex3-4

# Compilatore e flags
CC=gcc
CFLAGS=-I$(INC_DIR) -I$(LIB_DIR) -Wall -Wextra -std=c99

# Regola principale
all: $(MAIN_EXEC) $(TEST_EXEC)

# Compilazione main
$(MAIN_EXEC): $(SRCS) $(MAIN)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(MAIN)

# Compilazione test con Unity
$(TEST_EXEC): $(SRCS) $(TEST) $(UNITY)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(TEST) $(UNITY)

# Pulizia
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
