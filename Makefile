CC = gcc
CSTD = c89

CFLAGS = -g -W -Wall -Werror -std=$(CSTD)
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lm -lpng -lz

MAIN = main.c

game: main.o obj.o trans.o
	$(CC) $(LDFLAGS) -o game main.o obj.o trans.o
	rm main.o obj.o trans.o

main.o: $(MAIN)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $(MAIN)

obj.o: obj.c
	$(CC) $(CFLAGS) -c obj.c

trans.o: trans.c
	$(CC) $(CFLAGS) -c trans.c

clean:
	rm main.o obj.o game
