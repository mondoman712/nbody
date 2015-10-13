CC = gcc

#GUILE_CFLAGS = `guile-config compile`
SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -c -W -Wall -Werror

#GUILE_LDFLAGS = `guile-config link`
SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -g -lm

MAIN = src/main.c

nbody: main.o	
	$(CC) $(LDFLAGS) $(SDL_LDFLAGS) -o nbody main.o
	rm main.o

main.o: $(MAIN) $(SCHEME_INTERFACE) 
	$(CC) $(CFLAGS) $(SDL_CFLAGS) src/main.c

clean:
	rm main.o nbody
