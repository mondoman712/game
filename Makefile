CSTD = c89

SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -g -W -Wall -Werror -std=$(CSTD) $(SDL_CFLAGS) $(GUILE_CFLAGS)

SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -lGL -lGLEW -lm -lpng -lz  $(SDL_LDFLAGS) $(GUILE_LDFLAGS)

SOURCES = $(wildcard src/*.c)
OBJECTS = $(notdir $(SOURCES:.c=.o))
EXECUTABLE = emetic

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	rm main.o f_obj.o trans.o f_png.o

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm main.o f_obj.o trans.o f_png.o emetic
