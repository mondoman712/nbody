CC = gcc
CFLAGS = -c -Wall `guile-config compile`
LDFLAGS = -g -lm -lSDL2 -lSDL2_gfx `guile-config link`

nbody: main.o scheme_interface.o
	$(CC) $(LDFLAGS) -o nbody main.o scheme_interface.o

main.o: main.c defs.h scheme_interface.h scheme_interface.c
	$(CC) $(CFLAGS) main.c

scheme_interface.o: scheme_interface.c scheme_interface.h defs.h main.c main.h
	$(CC) $(CFLAGS) scheme_interface.c

clean:
	rm main.o scheme_interface.o nbody
