#include <stdio.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialise SDL\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
