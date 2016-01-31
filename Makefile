CC = gcc
CSTD = c99

GUILE_CFLAGS = $(shell pkg-config guile-2.0 --cflags)
SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -g -W -Wall -pedantic -std=$(CSTD) $(SDL_CFLAGS) $(GUILE_CFLAGS)

GUILE_LDFLAGS = $(shell pkg-config guile-2.0 --libs)
SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -lGL -lGLEW -lm -lpng -lz  $(SDL_LDFLAGS) $(GUILE_LDFLAGS)

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
