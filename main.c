#include <stdio.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 640
#define DEFAULT_SCREEN_Y 480

#define GL_MAJOR_VER 3
#define GL_MINOR_VER 3

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialise SDL\n");
		exit(EXIT_FAILURE);
	}

	SDL_Window * mainwin;
	mainwin = SDL_CreateWindow(
			WIN_TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			DEFAULT_SCREEN_X,
			DEFAULT_SCREEN_Y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (mainwin == NULL) {
		fprintf(stderr, "Failed to create SDL window\n");
		exit(EXIT_FAILURE);
	}


	SDL_GLContext gl_context = SDL_GL_CreateContext(mainwin);
	if (gl_context == NULL) {
		fprintf(stderr, "Failed to create OpenGL context\n");
		exit(EXIT_FAILURE);
	}

	const unsigned char * version = glGetString(GL_VERSION);
	if (version == NULL) {
		fprintf(stderr, "Failed to get GL version\n");
		exit(EXIT_FAILURE);
	}

	SDL_GL_MakeCurrent(mainwin, gl_context);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR_VER);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR_VER);

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status) {
		fprintf(stderr, "Error %s\n", glewGetErrorString(glew_status));
		exit(EXIT_FAILURE);
	}

	float vertices[] = {
		0.0, 0.5,
		0.5, -0.5,
		-0.5, -0.5
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLchar * _vertex_source = malloc(4096);
	FILE * vs = fopen("vs1.glsl", "r");
	if (vs == NULL) {
		fprintf(stderr, "Couldn't read vs1\n");
		exit(EXIT_FAILURE);
	}
	fread(_vertex_source, 512, 8, vs);
	fclose(vs);

	const GLchar * vertex_source = _vertex_source;
	free(_vertex_source);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	GLchar * _fragment_source = malloc(4096);
	FILE * fs = fopen("fs1.glsl", "r");
	if (fs == NULL) {
		fprintf(stderr, "Couldn't read fs1\n");
		exit(EXIT_FAILURE);
	}
	fread(_fragment_source, 512, 8, vs);
	fclose(fs);

	const GLchar * fragment_source = _fragment_source;
	free(_fragment_source);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glBindFragDataLocation(shader_program, 0, "out_colour");

	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLint pos_attrib = glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);

	SDL_Event e;
	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) 
				break;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_q)
				break;
		}

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mainwin);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}
