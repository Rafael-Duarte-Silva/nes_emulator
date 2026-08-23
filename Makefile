CC = gcc

CFLAGS = -g -Wall -Wextra -std=c11 -Isrc

SOURCEDIR = src/
TESTDIR = tests/
BINDIR = bin/

HEADERS_FP := $(shell find $(SOURCEDIR) -type f -name "*.h")
SOURCE_FP := $(shell find $(SOURCEDIR) -type f -name "*.c")

OBJECTS := $(patsubst $(SOURCEDIR)%.c,$(BINDIR)%.o,$(SOURCE_FP))

EXECUTABLE = $(BINDIR)main

# =========================================================
# TESTS
# =========================================================

CPU_TEST_EXECUTABLE = $(BINDIR)test_cpu
PPU_TEST_EXECUTABLE = $(BINDIR)test_ppu
CARTRIGDE_TEST_EXECUTABLE = $(BINDIR)test_cartrigde

CPU_TEST_OBJECTS = \
	$(BINDIR)test_cpu.o \
	$(BINDIR)cpu.o

PPU_TEST_OBJECTS = \
	$(BINDIR)test_ppu.o \
	$(BINDIR)ppu.o

CARTRIGDE_TEST_OBJECTS = \
	$(BINDIR)test_cartrigde.o \
	$(BINDIR)cartrigde.o 

.PHONY: main clean test test_cpu test_cartrigde test_ppu

$(BINDIR)test_%.o: $(TESTDIR)test_%.c $(HEADERS_FP) | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# PROGRAM
# =========================================================

main: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@

$(BINDIR)%.o: $(SOURCEDIR)%.c $(HEADERS_FP)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# CPU TEST
# =========================================================

test_cpu: $(CPU_TEST_EXECUTABLE)
	./$(CPU_TEST_EXECUTABLE)

$(CPU_TEST_EXECUTABLE): $(CPU_TEST_OBJECTS) | $(BINDIR)
	$(CC) $(CPU_TEST_OBJECTS) -o $@

# =========================================================
# PPU TEST
# =========================================================

test_ppu: $(PPU_TEST_EXECUTABLE)
	./$(PPU_TEST_EXECUTABLE)

$(PPU_TEST_EXECUTABLE): $(PPU_TEST_OBJECTS) | $(BINDIR)
	$(CC) $(PPU_TEST_OBJECTS) -o $@

# =========================================================
# CARTRIGDE TEST
# =========================================================

test_cartrigde: $(CARTRIGDE_TEST_EXECUTABLE)
	./$(CARTRIGDE_TEST_EXECUTABLE)

$(CARTRIGDE_TEST_EXECUTABLE): $(CARTRIGDE_TEST_OBJECTS) | $(BINDIR)
	$(CC) $(CARTRIGDE_TEST_OBJECTS) -o $@

# =========================================================
# RUN ALL TESTS
# =========================================================

test: test_cpu test_cartrigde test_ppu

# =========================================================
# BIN
# =========================================================

$(BINDIR):
	mkdir -p $(BINDIR)

# =========================================================
# CLEAN
# =========================================================

clean:
	rm -rf $(BINDIR)