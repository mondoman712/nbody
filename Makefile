CC = gcc
CFLAGS = -c -Wall
GUILE_CFLAGS = `guile-config compile`
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = -g -lm $(shell sdl2-config --libs) `guile-config link` 

MAIN = src/main.c src/main.h
SCHEME_INTERFACE = src/scheme_interface.c src/scheme_interface.h
DRAW = src/draw.c src/draw.h
BODY = src/body.c src/body.h
BODYDEFS = src/bodydefs.h

nbody: main.o scheme_interface.o draw.o body.o
	$(CC) $(LDFLAGS) -o nbody main.o scheme_interface.o
	rm main.o scheme_interface.o

main.o: $(MAIN) $(SCHEME_INTERFACE) 
	$(CC) $(CFLAGS) $(GUILE_CFLAGS) $(SDL_CFLAGS) src/main.c

scheme_interface.o: $(MAIN) $(SCHEME_INTERFACE) $(DRAW)
	$(CC) $(CFLAGS) $(GUILE_CFLAGS) $(SDL_CFLAGS) src/scheme_interface.c

draw.o: $(MAIN) $(DRAW)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) src/draw.c

body.o: $(BODYDEFS) $(BODY)
	$(CC) $(CFLAGS) src/body.c
clean:
	rm main.o scheme_interface.o nbody
