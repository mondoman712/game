#include <stdio.h>
#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "f_png.h"
#include "trans.h"
#include "f_obj.h"

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 960
#define DEFAULT_SCREEN_Y 960

#define PI 3.141592653589

#define GL_MAJOR_VER 3
#define GL_MINOR_VER 3

#define SHADER_DIR "shaders/"
#define SHADER_EXT ".glsl"

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

/*
 * Handles key up event in main loop
 */
static GLuint handle_keyup(SDL_Event e)
{
	switch(e.key.keysym.sym) {
	case (SDLK_q):
		return 1;
	case (SDLK_F10):
		/* TODO: Take screenshot */
		break;
	}
	
	return 0;
}

/*
 * Main
 */
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

	GLfloat * verts = malloc(150 * 3 * sizeof(GLfloat));
	GLuint * faces = malloc(256 * 3 * sizeof(GLuint));
	if (verts == NULL || faces == NULL) {
		fprintf(stderr, "Failed to allocate memory 01\n");
		exit(EXIT_FAILURE);
	}
	read_obj("assets/models/monkey.obj", verts, faces);

	int i;
	printf("vertices: %i\n", (int) *verts);
	for (i = 1; i < (*verts * 3); i += 3) {
		printf("%f, ", *(verts + i));
		printf("%f, ", *(verts + i + 1));
		printf("%f\n", *(verts + i + 2));
	}

	printf("faces: %i\n", (int) *faces);
	for (i = 1; i < (int) (*faces * 3); i += 3) {
		printf("%d, ", *(faces + i));
		printf("%d, ", *(faces + i + 1));
		printf("%d\n", *(faces + i + 2));
	}

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (*verts * 3 * sizeof(GLfloat)),
			verts + 1, GL_STATIC_DRAW);	

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*faces * 3 * sizeof(GLuint)),
			faces + 1, GL_STATIC_DRAW);

	GLuint vert_shader = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glBindFragDataLocation(shader_prog, 0, "out_colour");
	glLinkProgram(shader_prog);
	glUseProgram(shader_prog);

	/*
	GLuint tex[2];
	glGenTextures(2, tex);
	GLuint w, h;
	png_byte * img_data;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	img_data = read_png("assets/textures/cat.png", &w, &h);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
			GL_UNSIGNED_BYTE, img_data);
	free(img_data);
	glUniform1i(glGetUniformLocation(shader_prog, "texpat"), 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	img_data = read_png("assets/textures/dog.png", &w, &h);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
			GL_UNSIGNED_BYTE, img_data);
	free(img_data);
	glUniform1i(glGetUniformLocation(shader_prog, "texbmo"), 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	*/

	GLuint pos_attr = glGetAttribLocation(shader_prog, "position");
	glEnableVertexAttribArray(pos_attr);
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), 0);

	/*
	GLuint col_attr = glGetAttribLocation(shader_prog, "in_colour");
	glEnableVertexAttribArray(col_attr);
	glVertexAttribPointer(col_attr, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	GLint tex_attr = glGetAttribLocation(shader_prog, "texcoord");
	glEnableVertexAttribArray(tex_attr);
	glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
	*/

	GLfloat view[16];
	vec3 eye = {3.2, 0.0, 0.0};
	vec3 cent = {0.0, 0.0, 0.0};
	vec3 up = {0.0, 0.0, 1.0};
	look_at(eye, cent, up, view);
	GLint uni_view = glGetUniformLocation(shader_prog, "view");
	glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);

	GLfloat proj[16];
	perspective(PI / 2, DEFAULT_SCREEN_X / DEFAULT_SCREEN_Y, 0.1, 100.0,
			proj);
	GLint uni_proj = glGetUniformLocation(shader_prog, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	GLfloat model[16];
	GLint uni_model = glGetUniformLocation(shader_prog, "model");
	GLfloat k = 0.0;

	glEnable(GL_DEPTH_TEST);

	SDL_Event e;
	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			else if (e.type == SDL_KEYUP && handle_keyup(e))
				break;
		}
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		rotatez((PI / 180) * (k += 0.01), model);
		glUniformMatrix4fv(uni_model, 1, GL_FALSE, model);

		glDrawElements(GL_TRIANGLES, (*faces * 3), GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(mainwin);
	}

	free(verts);
	free(faces);

	glDeleteProgram(shader_prog);
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mainwin);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}
