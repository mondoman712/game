CSTD = c89
CFLAGS = -g -W -Wall -Werror -std=$(CSTD)
SDL_CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lm -lpng -lz

SOURCES = src/main.c src/f_obj.c src/trans.c src/f_png.c
OBJECTS = $(SOURCES: .c=.o)
EXECUTABLE = emetic

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm main.o f_obj.o trans.o f_png.o game
