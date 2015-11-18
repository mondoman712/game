#include <stdio.h>
#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <zlib.h>
#include <png.h>

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 1280
#define DEFAULT_SCREEN_Y 720

#define GL_MAJOR_VER 3
#define GL_MINOR_VER 3

#define SHADER_DIR "shaders/"
#define SHADER_EXT ".glsl"

/*
 * Checks the signature of the png file being read at stream
 */
GLuint readpng_checksig (FILE * stream)
{
	GLubyte sig[8];
	fread(sig, 1, 8, stream);
	return !png_check_sig(sig, 8);
}

GLuint readpng_init (char * filename, GLuint * width, GLuint * height)
{
	GLuint err = 0;

	/* Read File */
	FILE * f = fopen(filename, "rb");
	if (f == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		return 1;
	}

	if (readpng_checksig(f)) {
		fprintf(stderr, "PNG signature for %s is invalid\n", filename);
		err += 1;
		goto cleanup;
	}

	printf("w: %i, h: %i\n", *width, *height);

	png_structp png_ptr;
	png_infop info_ptr;

cleanup:
	fclose(f);
	return err;
}

/*
 * Reads and compiles a .glsl shader file in the shaders folder, from just the
 * core of the filename (to use shaders/vs1.glsl, filename is just vsl)
 */
GLuint create_shader (const GLenum shader_type, const char * filename)
{
	char _source[4096];
	char dest[64];

	strcpy(dest, SHADER_DIR);
	strcat(dest, filename);
	strcat(dest, SHADER_EXT);

	FILE * s = fopen(dest, "r");
	if (s == NULL) {
		fprintf(stderr, "Could not open file %s\n", dest);
		return 1;
	}

	int i = 0;
	char inc;
	while (fscanf(s, "%c", &inc) > 0)
		_source[i++] = inc;
	
	_source[i - 1] = '\0';

	const char * source = _source;
	fclose(s);

	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE) {
		printf("Shader %s has been compiled\n", filename);
	} else {
		char buff[512];
		glGetShaderInfoLog(shader, 512, NULL, buff);
		fprintf(stderr, "Shader %s failed to compile with error:\n", 
				filename);
		fprintf(stderr, "%s\n", buff);
		fprintf(stderr, "\n");
	}

	return shader;
}

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
	} else {
		printf("SDL window created\n");
	}


	SDL_GLContext gl_context = SDL_GL_CreateContext(mainwin);
	if (gl_context == NULL) {
		fprintf(stderr, "Failed to create OpenGL context\n");
		exit(EXIT_FAILURE);
	} else {
		printf("OpenGL context created\n");
	}

	const unsigned char * version = glGetString(GL_VERSION);
	if (version == NULL) {
		fprintf(stderr, "Failed to get GL version\n");
		exit(EXIT_FAILURE);
	} else {
		printf("GL version is: %s\n", version);
	}

	SDL_GL_MakeCurrent(mainwin, gl_context);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR_VER);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR_VER);

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status) {
		fprintf(stderr, "Error %s\n", glewGetErrorString(glew_status));
		exit(EXIT_FAILURE);
	} else {
		printf("GLEW is working\n");
	}

	GLfloat verts[] = {
		-0.5, 	 0.5, 	1.0, 	0.0, 	0.0,
		 0.5, 	 0.5, 	0.0, 	1.0, 	0.0,
		 0.5, 	-0.5, 	0.0, 	0.0, 	1.0,
		-0.5, 	-0.5, 	1.0, 	1.0, 	1.0
	};

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts,
			GL_STATIC_DRAW);	

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
			GL_STATIC_DRAW);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLfloat col[] = {1.0, 0.0, 0.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, col);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLfloat pixels[] = {
		0.0, 0.0, 0.0, 	1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 	0.0, 0.0, 0.0
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

	GLuint vert_shader = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glBindFragDataLocation(shader_prog, 0, "out_colour");
	glLinkProgram(shader_prog);
	glUseProgram(shader_prog);

	GLuint pos_attr = glGetAttribLocation(shader_prog, "position");
	glEnableVertexAttribArray(pos_attr);
	glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE,
		       5 * sizeof(GLfloat), 0);

	GLuint col_attr = glGetAttribLocation(shader_prog, "in_colour");
	glEnableVertexAttribArray(col_attr);
	glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

	SDL_Event e;
	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_q)
				break;
		}
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(mainwin);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mainwin);
	SDL_Quit();

	printf("Compiled with libpng %s; using libpng %s\n",
			PNG_LIBPNG_VER_STRING, png_libpng_ver);
	printf("Compiled with zlib %s; using zlib %s\n",
			ZLIB_VERSION, zlib_version);

	GLuint w = 1, h = 0;
	printf("%i\n", readpng_init("pat.png", &w, &h));

	exit(EXIT_SUCCESS);
}
