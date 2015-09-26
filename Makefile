CC = gcc

CFLAGS = -g -Wall -lSDL2 -lSDL2_gfx `pkg-config --cflags guile-2.0`
LIBS = `guile-config link`

PHONY: clean build run

TARGET = nbody
MAIN = main

all: $(TARGET)

build: $(TARGET)

$(TARGET): $(MAIN).o
	#$(CC) $< -o $@ $(LIBS)
	$(CC) -o $(TARGET) $(MAIN).c $(CFLAGS) 

$(MAIN).o: $(MAIN).c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
