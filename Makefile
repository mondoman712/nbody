CC = gcc

CFLAGS = -g -Wall -lSDL -lSDL_gfx 

TARGET = nbody

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
