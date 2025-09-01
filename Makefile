CC=gcc

CFLAGS=-c

HEADERDIR= src/
SOURCEDIR= src/

HEADER_FILES= types.h console.h cpu.h bus.h cartrigde.h mapper.h mapper0.h mapper1.h
SOURCE_FILES= main.c console.c cpu.c bus.c cartrigde.c mapper.c mapper0.c mapper1.c

HEADERS_FP = $(addprefix $(HEADERDIR),$(HEADER_FILES))
SOURCE_FP = $(addprefix $(SOURCEDIR),$(SOURCE_FILES))

OBJECTS =$(SOURCE_FP:.c = .o)

EXECUTABLE=main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(SDL_LFLAGS) -o $(EXECUTABLE)

%.o: %.c $(HEADERS_FP)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf src/*.o $(EXECUTABLE)