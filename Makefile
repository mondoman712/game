CC = gcc
CSTD = c99

CFLAGS =  -g -W -Wall -Werror -std=$(CSTD) $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lm

MAIN = main.c

game: main.o
	$(CC) $(LDFLAGS) -o game main.o
	rm main.o

main.o: $(MAIN)
	$(CC) $(CFLAGS) -c $(MAIN)

clean:
	rm main.o game
