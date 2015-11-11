#include <stdio.h>
#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "obj.h"

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 960
#define DEFAULT_SCREEN_Y 960

#define PI 3.141592653589

#define GL_MAJOR_VER 3
#define GL_MINOR_VER 3

#define SHADER_DIR "shaders/"
#define SHADER_EXT ".glsl"

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3;

/*
 * finds the modulus of a 1x3 vector
 */
static GLfloat mod_vec3 (vec3 in)
{
	return sqrt((in.x * in.x) + (in.y  * in.y) + (in.z * in.z));
}

/*
 * Finds the normal of a 1x3 vector
 */
static vec3 norm_vec3 (vec3 in)
{
	GLfloat mod = mod_vec3(in);
	vec3 ret = {
		in.x / mod,
		in.y / mod,
		in.z / mod
	};

	return ret;
}

/*
 * Finds the cross product of vectors a and b
 */
static vec3 cross_vec3 (vec3 a, vec3 b)
{
	vec3 ret;

	ret.x = (a.y * b.z) - (a.z * b.y);
	ret.y = (a.z * b.x) - (a.x * b.z);
	ret.z = (a.x * b.y) - (a.y * b.x);

	return ret;
}

/*
 * Deposits the values in a vec3 into the first 3 places in an array
 */
static void vec3_array (vec3 in, GLfloat * array)
{
	*(array) = in.x;
	*(array + 1) = in.y;
	*(array + 2) = in.z;
}

static void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4)
{
	vec3 f, s, u;

	f.x = centre.x - eye.x;
	f.y = centre.y - eye.y;
	f.z = centre.z - eye.z;

	f = norm_vec3(f);
	up = norm_vec3(up);

	s = cross_vec3(f, up);
	u = cross_vec3(norm_vec3(s), f);

	vec3_array(s, mat4);
	*(mat4 + 3) = 0;
	vec3_array(u, (mat4 + 4));
	*(mat4 + 7) = 0;

	f.x = -f.x;
	f.y = -f.y;
	f.z = -f.z;

	vec3_array(f, (mat4 + 8));
	*(mat4 + 11) = 0;
	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1;
}

static void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4)
{
	GLfloat f = 1 / tan(fovy / 2);
	*mat4 = f / asp;
	*(mat4 + 1) = 0;
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = 0;
	*(mat4 + 5) = f;
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = (znear + zfar) / (znear - zfar);
	*(mat4 + 11) = (2 * zfar * znear) / (znear - zfar);

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = -1;
	*(mat4 + 15) = 0;
}

static void rotatez (GLfloat ang, GLfloat * mat4)
{
	*mat4 = cos(ang);
	*(mat4 + 1) = -sin(ang);
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = sin(ang);
	*(mat4 + 5) = cos(ang);
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = 1;
	*(mat4 + 11) = 0;

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1;
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

	/*
	float vertices[] = {
		-0.5,   0.5,  1.0,  0.0,  0.0,
		 0.5,  -0.5,  0.0,  1.0,  0.0,
		-0.5,  -0.5,  0.0,  0.0,  1.0,
		 0.5,   0.5,  1.0,  1.0,  1.0,
	};

	size_t nverts = sizeof(vertices) / sizeof(float);
	*/

	float * vertices = malloc(128 * sizeof(float));
	size_t nverts = read_obj("models/sphere.obj", vertices);

	printf("nverts: %i\n", (int)nverts);
	int i;
	for (i = 0; i < (int) (nverts * 3); i += 3) {
		printf("%f, ", *(vertices + i));
		printf("%f, ", *(vertices + i + 1));
		printf("%f\n", *(vertices + i + 2));
	}

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(float), vertices,
			GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glBindFragDataLocation(shader_program, 0, "out_colour");

	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	GLint pos_attrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 
			3 * sizeof(GLfloat), 0);

	GLfloat view[16];
	vec3 eye = {1.2, 1.2, 1.2};
	vec3 cent = {0.0, 0.0, 0.0};
	vec3 up = {0.0, 0.0, 1.0};
	look_at(eye, cent, up, view);
	GLint uni_view = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);

	GLfloat proj[16];
	perspective(PI / 2, DEFAULT_SCREEN_X / DEFAULT_SCREEN_Y, 0.1, 100.0,
			proj);
	GLint uni_proj = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	GLfloat trans[16];
	rotatez(PI / 4, trans);
	GLint uni_trans = glGetUniformLocation(shader_program, "trans");
	glUniformMatrix4fv(uni_trans, 1, GL_FALSE, trans);
	/*
	GLint col_attrib = glGetAttribLocation(shader_program, "in_colour");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	*/

	SDL_Event e;
	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) 
				break;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_q)
				break;
		}
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_LINE_LOOP, 0, (int)(nverts * 3));

		SDL_GL_SwapWindow(mainwin);
	}

	glDeleteProgram(shader_program);
	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mainwin);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}
