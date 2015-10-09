CC = gcc

#GUILE_CFLAGS = `guile-config compile`
#SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -c -Wall

#GUILE_LDFLAGS = `guile-config link`
#SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -g -lm

MAIN = src/main.c

nbody: main.o	
	$(CC) $(LDFLAGS) -o nbody main.o
	rm main.o

main.o: $(MAIN) $(SCHEME_INTERFACE) 
	$(CC) $(CFLAGS) src/main.c

clean:
	rm main.o scheme_interface.o body.o nbody
