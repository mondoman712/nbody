CC = gcc

CFLAGS = -g -Wall -lSDL2 -lSDL2_gfx 

TARGET = nbody
MAIN = main.c

all: $(TARGET)

$(TARGET): $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)
