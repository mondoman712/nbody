CC = gcc

CFLAGS = -g -Wall -lSDL2 -lSDL2_gfx `pkg-config --cflags guile-2.0`

TARGET = nbody
MAIN = main.c

all: $(TARGET)

$(TARGET): $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)
