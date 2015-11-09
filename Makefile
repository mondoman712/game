CC = gcc
CSTD = c89

CFLAGS = -g -W -Wall -Werror -std=$(CSTD)
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lm

MAIN = main.c

game: main.o obj.o
	$(CC) $(LDFLAGS) -o game main.o obj.o
	rm main.o obj.o

main.o: $(MAIN)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $(MAIN)

obj.o: obj.c
	$(CC) $(CFLAGS) -c obj.c

clean:
	rm main.o obj.o game
