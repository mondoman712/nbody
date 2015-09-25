CC = gcc

CFLAGS = -g -Wall -lSDL2 -lSDL2_gfx -lpthread `guile-config compile`
LIBS = `guile-config link`

TARGET = nbody
MAIN = main.c

all: $(TARGET)

$(TARGET): $(MAIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN)

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
