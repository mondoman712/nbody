CC = gcc

CFLAGS = -c -g -Wall -lSDL2 -lSDL2_gfx `pkg-config --libs --cflags guile-2.0`
LIBS = `guile-config link`

TARGET = nbody

all: $(TARGET)

build: $(TARGET)

$(TARGET): main.o scheme_interface.o
	$(CC) main.o scheme_interface.o -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) main.c

scheme_interface.o: scheme_interface.c
	$(CC) $(CFLAGS) scheme_interface.c

clean:
	$(RM) $(TARGET) *.o

run:
	./$(TARGET)
