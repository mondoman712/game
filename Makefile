CSTD = c89
CFLAGS = -g -W -Wall -Werror -std=$(CSTD)
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lm -lpng -lz

SOURCES = main.c obj.c trans.c f_png.c
OBJECTS = $(SOURCES: .c=.o)
EXECUTABLE = game

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm main.o obj.o trans.o f_png.o game
