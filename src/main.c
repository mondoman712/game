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
#define GL_MINOR_VER 3

#define SHADER_DIR "src/shaders/"
#define SHADER_EXT ".glsl"

typedef struct {
	GLuint program;
	GLuint vert;
	GLuint tex;
	GLuint norm;
	GLuint model;
	GLuint matshine;
	GLuint matcol;
	GLuint illum;
} attrib;

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

	/* Open file */
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
	
	/* Close file */
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
 * Create filename for screenshot
 */
static char * ss_filename (void)
{
	struct tm * tm;
	time_t t;
	char strtime[128];

	char * filename = malloc(128 * sizeof(char));
	GLushort i = 1;
	
	/* Create filename based on time */
	t = time(NULL);
	tm = localtime(&t);
	strftime(strtime, sizeof(strtime), "%Y-%m-%d-%H-%M-%S", tm);
	sprintf(filename, "screenshot-%s.png", strtime);

	/* If another file of this name exists add digits to the end */
	while (access(filename, F_OK) != -1)
		sprintf(filename, "screenshot-%s-%d.png", strtime, i++);

	return filename;
}

/*
 * Takes a screenshot, saves it to scrn-yyyy-mm-dd-hh-mm-ss(-x).png
 */
static GLuint take_screenshot (GLuint w, GLuint h)
{
	GLuint ret = 0;
	GLubyte * pix = malloc(w * h * 3 * sizeof(GLubyte));
	char * filename;

	/* Get pixel data from OpenGL */
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) pix);

	/* Get filename */
	filename = ss_filename();
	
	/* Save the file and print out a message */
	if (access(filename, F_OK) == -1) {
		if (save_png(filename, pix, w, h)) { 
			ret = 1;
			fprintf(stderr, "Screenshot Failed\n");
		} else {
			printf("\nTaken screenshot %s\n", filename);
		}
	}

	free(pix);
	free(filename);

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
 * Draws the given object to the screen
 */
static void draw_object (object obj, attrib attr)
{
	glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);

	glBindTexture(GL_TEXTURE_2D, obj.mat.tex);

	glUniform1i(attr.illum, obj.mat.illum);

	GLfloat model[16];
	trans_rot(obj.pos, obj.rot, model);
	glUniformMatrix4fv(attr.model, 1, GL_FALSE, model);

	glVertexAttribPointer(attr.vert, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(attr.tex, 2, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(attr.norm, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, (GLuint) *obj.verts);
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
	mainwin = SDL_CreateWindow(WIN_TITLE,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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

	GLuint vert_shader = create_shader(GL_VERTEX_SHADER, "vs1");
	GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, "fs1");

	GLuint shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glBindFragDataLocation(shader_prog, 0, "out_colour");
	glLinkProgram(shader_prog);
	glUseProgram(shader_prog);

	object skybox = build_obj("skybox", shader_prog);
	object monkey = build_obj("monkey", shader_prog);
	attrib attr;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLint uni_lightp = glGetUniformLocation(shader_prog, "light.position");
	glUniform3f(uni_lightp, 1.0, 1.0, 1.0);
	GLint uni_lighti = glGetUniformLocation(shader_prog, "light.intensities");
	glUniform3f(uni_lighti, 1.0, 1.0, 1.0);
	GLint uni_lighta = glGetUniformLocation(shader_prog, "light.attenuation");
	glUniform1f(uni_lighta, 0.2);
	GLint uni_lightc = glGetUniformLocation(shader_prog, 
			"light.ambient_coefficient");
	glUniform1f(uni_lightc, 0.005);

	GLint uni_matshine = glGetUniformLocation(shader_prog, "mat_shine");
	glUniform1f(uni_matshine, skybox.mat.shine);
	GLint uni_matcol = glGetUniformLocation(shader_prog, "mat_specularcol");
	glUniform3f(uni_matcol, skybox.mat.spec_col.x, skybox.mat.spec_col.y,
			skybox.mat.spec_col.z);
	attr.illum = glGetUniformLocation(shader_prog, "illum");

	GLint uni_campos = glGetUniformLocation(shader_prog, "campos");

	attr.vert = glGetAttribLocation(shader_prog, "vert");
	glEnableVertexAttribArray(attr.vert);

	attr.tex = glGetAttribLocation(shader_prog, "verttexcoord");
	glEnableVertexAttribArray(attr.tex);

	attr.norm = glGetAttribLocation(shader_prog, "vertnorm");
	glEnableVertexAttribArray(attr.norm);

	GLfloat view[16];
	vec3 eye = {4.0, 0.0, 0.0};
	GLint uni_view = glGetUniformLocation(shader_prog, "view");
	glUniform3f(uni_campos, eye.x, eye.y, eye.z);

	GLfloat fov = PI / 2;
	GLfloat proj[16];
	perspective(fov, w / h, 0.1, 100.0, proj);
	GLint uni_proj = glGetUniformLocation(shader_prog, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	attr.model = glGetUniformLocation(shader_prog, "model");
	clock_t k;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	int mx = 0, my = 0;
	GLfloat pitch = 0;
       	GLfloat yaw = PI / 2;
	GLfloat sens = 1;
	GLushort pause = 0;

	glEnable(GL_DEPTH_TEST);

	Uint64 ts, te;
	double tpf;
	/* Add some text that I can overwrite later */
	printf("0.000000");

	SDL_Event e;
	while (1) {
		ts = SDL_GetPerformanceCounter();

		/* Handle Events */
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			} else if (e.type == SDL_KEYUP) {
				if (handle_keyup(e, w, h, &pause)) break;
			} else if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_resize(mainwin, &w, &h);
				perspective(fov, (GLfloat) w / (GLfloat) h,
						0.1, 100.0, proj);
				glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);
			}
		}

		if (!pause) {
			/* Set Screen to black */
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			/* Rotation of monkey based on time */
			k = clock();

			monkey.rot = (vec3) {-k / 1000000.0, 0, k / 1000000.0};
			
			/* Handle mouse movement */
			SDL_GetRelativeMouseState(&mx, &my);
			pitch -= ((GLfloat) my / (GLfloat) h) * sens;
			yaw -= ((GLfloat) mx / (GLfloat) w) * sens;
			look_to(eye, pitch, yaw, view);
			glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);
		}

		/* Draw objects */
		draw_object(skybox, attr);
		draw_object(monkey, attr);

		SDL_GL_SwapWindow(mainwin);
	
		te = SDL_GetPerformanceCounter() - ts;
		tpf = (double) te / (double) SDL_GetPerformanceFrequency() * 1000;
		printf("\r%1.6fms", tpf);
	}
	printf("\n");

	free(skybox.verts);

	glDeleteProgram(shader_prog);
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	glDeleteBuffers(1, &skybox.vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(gl_context); SDL_DestroyWindow(mainwin);
	SDL_Quit();

	;exit(EXIT_SUCCESS);
}
