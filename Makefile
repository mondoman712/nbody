CC = gcc
CFLAGS = -c -Wall $(shell sdl2-config --cflags) `guile-config compile`
LDFLAGS = -g -lm $(shell sdl2-config --libs) `guile-config link` 

MAIN = src/main.c
DEFS = src/defs.h 
SCHEME_INTERFACE = src/scheme_interface.c src/scheme_interface.h
DRAW = src/draw.c src/draw.h

nbody: main.o scheme_interface.o
	$(CC) $(LDFLAGS) -o nbody main.o scheme_interface.o
	rm main.o scheme_interface.o

main.o: $(MAIN) $(SCHEME_INTERFACE) $(DEFS)
	$(CC) $(CFLAGS) src/main.c

scheme_interface.o: $(MAIN) $(DEFS) $(SCHEME_INTERFACE) $(DRAW)
	$(CC) $(CFLAGS) src/scheme_interface.c

clean:
	rm main.o scheme_interface.o nbody
