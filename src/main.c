#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <libguile.h>

#include "f_png.h"
#include "trans.h"
#include "f_obj.h"

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 960
#define DEFAULT_SCREEN_Y 960

#define PI 3.141592653589

#define GL_MAJOR_VER 3
#define GL_MINOR_VER 0

#define SHADER_DIR "src/shaders/"
#define SHADER_EXT ".glsl"

/*
 * Reads and compiles a .glsl shader file in the shaders folder, from just the
 * core of the filename (to use shaders/vs1.glsl, filename is just vsl)
 */
GLuint create_shader (const GLenum shader_type, const char * filename)
{
	char _source[4096];
	char dest[64];

	/* Create full filename */
	strcpy(dest, SHADER_DIR);
	strcat(dest, filename);
	strcat(dest, SHADER_EXT);

	FILE * s = fopen(dest, "r");
	if (s == NULL) {
		fprintf(stderr, "Could not open file %s\n", dest);
		return 1;
	}

	/* Read all lines in file */
	int i = 0;
	char inc;
	while (fscanf(s, "%c", &inc) > 0)
		_source[i++] = inc;
	
	fclose(s);

	/* Add string terminator to file */
	_source[i - 1] = '\0';

	/* Change type of source */
	const char * source = _source;

	/* Compile Shader */
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	/* Check for and report errors */
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
 * Takes a screenshot, saves it to scrn-yyyy-mm-dd-hh-mm-ss(-x).png
 */
static GLuint take_screenshot (GLuint w, GLuint h)
{
	GLuint ret = 0;
	GLubyte * pix = malloc(w * h * 3 * sizeof(GLubyte));

	struct tm * tm;
	time_t t;
	char strtime[128];

	GLushort i = 1;
	char filename[128];
	
	/* Get pixel data from OpenGL */
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) pix);
	
	/* Create filename based on time */
	t = time(NULL);
	tm = localtime(&t);
	strftime(strtime, sizeof(strtime), "%Y-%m-%d-%H-%M-%S", tm);
	printf("%s\n", strtime);
	sprintf(filename, "screenshot-%s.png", strtime);

	/* If another file of this name exists add digits to the end */
	while (access(filename, F_OK) != -1)
		sprintf(filename, "screenshot-%s-%d.png", strtime, i++);

	/* Save the file and print out a message */
	if (access(filename, F_OK) == -1) {
		if (save_png(filename, pix, w, h)) { 
			ret = 1;
			fprintf(stderr, "Screenshot Failed\n");
		} else {
			printf("Taken screenshot %s\n", filename);
		}
	}

	free(pix);
	return ret;
}

/*
 * Handles key up event in main loop
 */
static GLuint handle_keyup (SDL_Event e, GLuint w, GLuint h, GLushort * pause)
{
	switch(e.key.keysym.sym) {
	case (SDLK_q):
		return 1;
	case (SDLK_F10):
		take_screenshot(w, h);
		break;
	case (SDLK_SPACE):
		if (*pause) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			*pause = 0;
		} else {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			*pause = 1;
		}
		break;
	}
	
	return 0;
}

/*
 * Handles window resize event
 */
static void window_resize (SDL_Window * window, GLuint * width,
		GLuint * height)
{
	GLint w, h;

	SDL_GetWindowSize(window, &w, &h);

	glViewport(0, 0, w, h);
	if (w > 0) *width = w;
	if (h > 0) *height = h;
}

/*
 * Main
 */
int main (void)
{
	scm_init_guile();

	GLuint w = DEFAULT_SCREEN_X;
	GLuint h = DEFAULT_SCREEN_Y;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialise SDL\n");
		exit(EXIT_FAILURE);
	}

	SDL_Window * mainwin;
	mainwin = SDL_CreateWindow(
			WIN_TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			w, h,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
				| SDL_WINDOW_RESIZABLE);
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

	GLfloat * verts = NULL;
	read_obj("assets/models/monkey.obj", &verts);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (int) (*verts * sizeof(GLfloat)),
			verts + 1, GL_STATIC_DRAW);	

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vert_shader = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glBindFragDataLocation(shader_prog, 0, "out_colour");
	glLinkProgram(shader_prog);
	glUseProgram(shader_prog);

	GLuint tex;
	glGenTextures(1, &tex);
	GLuint tw, th;
	png_byte * img_data;

	glBindTexture(GL_TEXTURE_2D, tex);
	img_data = read_png("assets/textures/dog.png", &tw, &th);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB,
			GL_UNSIGNED_BYTE, img_data);
	free(img_data);
	glUniform1i(glGetUniformLocation(shader_prog, "tex"), 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint pos_attr = glGetAttribLocation(shader_prog, "position");
	glEnableVertexAttribArray(pos_attr);
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), 0);

	GLint tex_attr = glGetAttribLocation(shader_prog, "texcoord");
	glEnableVertexAttribArray(tex_attr);
	glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	GLfloat view[16];
	vec3 eye = {4.0, 0.0, 0.0};
	GLint uni_view = glGetUniformLocation(shader_prog, "view");

	GLfloat fov = PI / 2;
	GLfloat proj[16];
	perspective(fov, w / h, 0.1, 100.0, proj);
	GLint uni_proj = glGetUniformLocation(shader_prog, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	GLfloat pos[16];
	vec3 ps = {0.0, 0.0, 0.0};
	translate(ps, pos);
	GLint uni_pos = glGetUniformLocation(shader_prog, "pos");
	glUniformMatrix4fv(uni_pos, 1, GL_FALSE, pos);

	GLfloat model[16];
	GLint uni_model = glGetUniformLocation(shader_prog, "model");
	clock_t k;

	glEnable(GL_DEPTH_TEST);

	SDL_SetRelativeMouseMode(SDL_TRUE);
	int mx = 0, my = 0;
	GLfloat pitch = 0, yaw = PI / 2;
	GLfloat sens = 1;
	GLushort p = 0;

	SDL_Event e;
	while (1) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			} else if (e.type == SDL_KEYUP) {
				if (handle_keyup(e, w, h, &p)) break;
			} else if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_resize(mainwin, &w, &h);
				perspective(fov, (GLfloat) w / (GLfloat) h,
						0.1, 100.0, proj);
				glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);
			}
		}

		if (!p) {
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			k = clock();
			rotate(k / 1000000.0, k / 1000000.0, k / 1000000.0,
					model);
			glUniformMatrix4fv(uni_model, 1, GL_FALSE, model);
			
			SDL_GetRelativeMouseState(&mx, &my);
			pitch -= ((GLfloat) my / (GLfloat) h) * sens;
			yaw -= ((GLfloat) mx / (GLfloat) w) * sens;
			look_to(eye, pitch, yaw, view);
			glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);
		}

		glDrawArrays(GL_TRIANGLES, 0, (GLuint) *verts);
		SDL_GL_SwapWindow(mainwin);
	}

	free(verts);

	glDeleteProgram(shader_prog);
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mainwin);
	SDL_Quit();

	printf("%s", find_tex("assets/models/cube.mtl"));

	exit(EXIT_SUCCESS);
}
