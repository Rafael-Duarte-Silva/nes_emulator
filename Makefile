CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Isrc

HEADERDIR = src/
SOURCEDIR = src/
TESTDIR = tests/
BINDIR = bin/

HEADER_FILES = types.h console.h cpu.h ppu.h bus.h cartrigde.h mapper.h mapper0.h
SOURCE_FILES = main.c console.c cpu.c ppu.c bus.c cartrigde.c mapper.c mapper0.c

HEADERS_FP = $(addprefix $(HEADERDIR),$(HEADER_FILES))
SOURCE_FP = $(addprefix $(SOURCEDIR),$(SOURCE_FILES))

OBJECTS = $(SOURCE_FILES:%.c=$(BINDIR)%.o)

EXECUTABLE = $(BINDIR)main

# =========================================================
# TESTS
# =========================================================

CPU_TEST_EXECUTABLE = $(BINDIR)test_cpu
CARTRIGDE_TEST_EXECUTABLE = $(BINDIR)test_cartrigde

CPU_TEST_OBJECTS = \
	$(BINDIR)test_cpu.o \
	$(BINDIR)cpu.o

CARTRIGDE_TEST_OBJECTS = \
	$(BINDIR)test_cartrigde.o \
	$(BINDIR)cartrigde.o \

.PHONY: all main clean test test_cpu test_cartrigde

$(BINDIR)test_%.o: $(TESTDIR)test_%.c $(HEADERS_FP) | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# PROGRAM
# =========================================================

all: $(EXECUTABLE)

main: $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@

$(BINDIR)%.o: $(SOURCEDIR)%.c $(HEADERS_FP) | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# CPU TEST
# =========================================================

test_cpu: $(CPU_TEST_EXECUTABLE)
	./$(CPU_TEST_EXECUTABLE)

$(CPU_TEST_EXECUTABLE): $(CPU_TEST_OBJECTS)
	$(CC) $(CPU_TEST_OBJECTS) -o $@

# =========================================================
# CARTRIGDE TEST
# =========================================================

test_cartrigde: $(CARTRIGDE_TEST_EXECUTABLE)
	./$(CARTRIGDE_TEST_EXECUTABLE)

$(CARTRIGDE_TEST_EXECUTABLE): $(CARTRIGDE_TEST_OBJECTS)
	$(CC) $(CARTRIGDE_TEST_OBJECTS) -o $@

# =========================================================
# RUN ALL TESTS
# =========================================================

test: test_cpu test_cartrigde

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