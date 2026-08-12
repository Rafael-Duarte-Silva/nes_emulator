CC = gcc

CFLAGS = -Wall -Wextra -std=c11

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
TEST_EXECUTABLE = $(BINDIR)test_cpu

TEST_SOURCE = $(TESTDIR)test_cpu.c
TEST_OBJECTS = $(BINDIR)test_cpu.o $(BINDIR)cpu.o

.PHONY: all test clean

# =========================================================
# PROGRAM
# =========================================================

all: $(EXECUTABLE)

main: $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) $(SDL_LFLAGS) $(LDFLAGS) -o $@

$(BINDIR)%.o: $(SOURCEDIR)%.c $(HEADERS_FP) | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# TESTES
# =========================================================

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS) | $(BINDIR)
	$(CC) $(TEST_OBJECTS) $(LDFLAGS) -o $@

$(BINDIR)%.o: $(TESTDIR)%.c $(HEADERS_FP) | $(BINDIR)
	$(CC) $(CFLAGS) -Isrc -c $< -o $@

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