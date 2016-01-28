CC = gcc
CSTD = c99

SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -g -W -Wall -pedantic -std=$(CSTD) $(SDL_CFLAGS)

SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -lGL -lGLEW -lm -lpng -lz  $(SDL_LDFLAGS)

SOURCES = $(wildcard src/*.c)
OBJECTS = $(notdir $(SOURCES:.c=.o))
EXECUTABLE = nbody

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	rm $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm $(OBJECTS) $(EXECUTABLE)
