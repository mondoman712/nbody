CC = gcc

CFLAGS = -g -Wall -lSDL2 -lSDL2_gfx 

TARGET = nbody

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
