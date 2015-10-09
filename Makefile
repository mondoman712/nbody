CC = gcc
CFLAGS = -c -Wall
GUILE_CFLAGS = `guile-config compile`
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = -g -lm $(shell sdl2-config --libs) `guile-config link` 

MAIN = src/main.c

nbody: main.o	
	$(CC) $(LDFLAGS) -o nbody main.o
	rm main.o

main.o: $(MAIN) $(SCHEME_INTERFACE) 
	$(CC) $(CFLAGS) src/main.c

clean:
	rm main.o scheme_interface.o body.o nbody
