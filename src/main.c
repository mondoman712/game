#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <libguile.h>

#include "slibs/g_lsl.h"
#include "slibs/f_png.h"
#include "slibs/v_3dt.h"
#include "f_obj.h"

#define WIN_TITLE "window title"
#define DEFAULT_SCREEN_X 960
#define DEFAULT_SCREEN_Y 960

#define PI 3.141592653589

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

typedef struct {
	vec3 pos;
	GLfloat pitch;
	GLfloat yaw;
} camera;

/*
 * Handles key up event in main loop
 */
static GLuint handle_keyup (SDL_Event e, GLuint w, GLuint h, GLushort * pause)
{
	switch (e.key.keysym.sym) {
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

static GLuint handle_keydown (const Uint8 * state, camera * cam, Uint64 dt)
{
	GLfloat ms = 10 * (double) dt / (double) SDL_GetPerformanceFrequency();

	if (state[SDL_SCANCODE_Q]) {
		return 1;
	}
	if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
		cam->pos.x -= ms * sin(cam->yaw);
		cam->pos.z -= ms * cos(cam->yaw);
	}
	if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
		cam->pos.x += ms * sin(cam->yaw);
		cam->pos.z += ms * cos(cam->yaw);
	}
	if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
		cam->pos.x -= ms * sin(cam->yaw + PI / 2);
		cam->pos.z -= ms * cos(cam->yaw + PI / 2);
	}
	if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
		cam->pos.x += ms * sin(cam->yaw + PI / 2);
		cam->pos.z += ms * cos(cam->yaw + PI / 2);
	}

	return 0;
}

/*
 * Draws the given object to the screen
 */
static void draw_object (object obj, attrib attr)
{
	glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);

	glBindTexture(GL_TEXTURE_2D, obj.mat.tex);

	glUniform1f(attr.matshine, obj.mat.shine);
	glUniform3f(attr.matcol, obj.mat.spec_col.x, obj.mat.spec_col.y,
			obj.mat.spec_col.z);
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

	Window mainwin = create_window(DEFAULT_SCREEN_X, DEFAULT_SCREEN_Y,
			WIN_TITLE);
	if (mainwin.win == NULL || mainwin.glc == NULL)
		exit(EXIT_FAILURE);

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
	monkey.pos = (vec3) {-4.0, 0.0, 0.0};
	object cube = build_obj("cube", shader_prog);
	cube.pos = (vec3) {2.0, 2.0, 0.0};
	attrib attr;
	object monkey2 = build_obj("monkey", shader_prog);
	monkey2.pos = (vec3) {4.0, 0.0, 0.0};

	GLint uni_lightp = glGetUniformLocation(shader_prog, "light.position");
	glUniform4f(uni_lightp, 1.0, 0.0, 0.0, 1.0);
	GLint uni_lighti = glGetUniformLocation(shader_prog, "light.intensities");
	glUniform3f(uni_lighti, 1.0, 1.0, 1.0);
	GLint uni_lighta = glGetUniformLocation(shader_prog, "light.attenuation");
	glUniform1f(uni_lighta, 0.2);
	GLint uni_lightc = glGetUniformLocation(shader_prog, 
			"light.ambient_coefficient");
	glUniform1f(uni_lightc, 0.005);

	attr.matshine = glGetUniformLocation(shader_prog, "mat_shine");
	attr.matcol = glGetUniformLocation(shader_prog, "mat_specularcol");
	attr.illum = glGetUniformLocation(shader_prog, "illum");

	GLint uni_campos = glGetUniformLocation(shader_prog, "campos");

	attr.vert = glGetAttribLocation(shader_prog, "vert");
	glEnableVertexAttribArray(attr.vert);

	attr.tex = glGetAttribLocation(shader_prog, "verttexcoord");
	glEnableVertexAttribArray(attr.tex);

	attr.norm = glGetAttribLocation(shader_prog, "vertnorm");
	glEnableVertexAttribArray(attr.norm);

	camera cam;
	GLfloat view[16];
	cam.pos = (vec3) {0.0, 0.0, 0.0};
	GLint uni_view = glGetUniformLocation(shader_prog, "view");
	glUniform3f(uni_campos, cam.pos.x, cam.pos.y, cam.pos.z);

	GLfloat fov = PI / 2;
	GLfloat proj[16];
	perspective(fov, mainwin.w / mainwin.h, 0.1, 100.0, proj);
	GLint uni_proj = glGetUniformLocation(shader_prog, "proj");
	glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);

	attr.model = glGetUniformLocation(shader_prog, "model");

	SDL_SetRelativeMouseMode(SDL_TRUE);
	int mx = 0, my = 0;
	cam.pitch = 0;
       	cam.yaw = PI / 2;
	GLfloat sens = 1;
	GLushort pause = 0;

	glEnable(GL_DEPTH_TEST);

	Uint64 ts, te;
	GLdouble tpf = 0;

	const Uint8 * state = SDL_GetKeyboardState(NULL);
	SDL_Event e;
	while (1) {
		ts = SDL_GetPerformanceCounter();

		/* Handle Events */
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			} else if (e.type == SDL_KEYUP) {
			} else if (e.type == SDL_KEYDOWN) {
			} else if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_resize(&mainwin);
				perspective(fov, (GLfloat) mainwin.w / (GLfloat) mainwin.h,
						0.1, 100.0, proj);
				glUniformMatrix4fv(uni_proj, 1, GL_FALSE, proj);
			}
		}
		if (handle_keyup(e, mainwin.w, mainwin.h, &pause)) break;
		if (handle_keydown(state, &cam, te)) break;
		glUniform3f(uni_campos, cam.pos.x, cam.pos.y,
				cam.pos.z);
		glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);

		/* Set Screen to black */
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!pause) {
			/* Rotation of monkey based on time */
			monkey.rot.x -= tpf / 1000;
			monkey.rot.z += tpf / 1000;
			monkey2.rot.x -= tpf / 1000;
			monkey2.rot.z += tpf / 1000;
			cube.rot.x += tpf / 1000;
			cube.pos.x += 10 * sin(tpf / 1000);
			cube.pos.z += 10 * cos(tpf / 1000);
			
			/* Handle mouse movement */
			SDL_GetRelativeMouseState(&mx, &my);
			cam.pitch -= ((GLfloat) my / (GLfloat) mainwin.h) * sens;
			if (cam.pitch > PI / 2)
				cam.pitch = PI / 2;
			else if (cam.pitch < - PI / 2)
				cam.pitch = - PI / 2;
			cam.yaw -= ((GLfloat) mx / (GLfloat) mainwin.w) * sens;
			look_to(cam.pos, cam.pitch, cam.yaw, view);
			glUniformMatrix4fv(uni_view, 1, GL_FALSE, view);
		}

		/* Draw objects */
		draw_object(skybox, attr);
		draw_object(monkey, attr);
		draw_object(cube, attr);
		draw_object(monkey2, attr);

		SDL_GL_SwapWindow(mainwin.win);
	
		te = SDL_GetPerformanceCounter() - ts;
		tpf = (double) te / (double) SDL_GetPerformanceFrequency() * 1000;
		printf("\r%02.6fms", tpf);
	}

	printf("\n");

	free(skybox.verts);

	glDeleteProgram(shader_prog);
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	glDeleteBuffers(1, &skybox.vbo);

	SDL_GL_DeleteContext(mainwin.glc); 
	SDL_DestroyWindow(mainwin.win);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}
