CSTD = c89

GUILE_CFLAGS = $(shell pkg-config guile-2.0 --cflags)
SDL_CFLAGS = $(shell sdl2-config --cflags)
CFLAGS = -g -W -Wall -std=$(CSTD) $(SDL_CFLAGS) $(GUILE_CFLAGS)

GUILE_LDFLAGS = $(shell pkg-config guile-2.0 --libs)
SDL_LDFLAGS = $(shell sdl2-config --libs)
LDFLAGS = -lGL -lGLEW -lm -lpng -lz  $(SDL_LDFLAGS) $(GUILE_LDFLAGS)

SOURCES = src/main.c src/trans.c src/f_obj.c src/f_png.c
OBJECTS = $(notdir $(SOURCES:.c=.o))
EXECUTABLE = emetic

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	rm main.o f_obj.o trans.o f_png.o

%.o: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm main.o f_obj.o trans.o f_png.o emetic
