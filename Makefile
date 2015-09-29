CC = gcc
CFLAGS = -c -Wall $(shell sdl2-config --cflags) `guile-config compile`
LDFLAGS = -g -lm $(shell sdl2-config --libs) `guile-config link` 
nbody: main.o scheme_interface.o
	$(CC) $(LDFLAGS) -o nbody main.o scheme_interface.o
	rm main.o scheme_interface.o

main.o: src/main.c src/defs.h src/scheme_interface.h src/scheme_interface.c
	$(CC) $(CFLAGS) src/main.c

scheme_interface.o: src/scheme_interface.c src/scheme_interface.h src/defs.h src/main.c src/main.h
	$(CC) $(CFLAGS) src/scheme_interface.c

clean:
	rm main.o scheme_interface.o nbody
