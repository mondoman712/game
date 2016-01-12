#ifndef _F_PNG_H
#define _F_PNG_H

#include <GL/glew.h>
#include <png.h>

typedef struct {
	GLuint w;
	GLuint h;
	png_byte * data;
	GLuint colour_type;
} image;

/*
 * Reads png file
 */
image read_png (const char * filename);

/*
 * Saves a byte array stored in pixels of width w and height h as a png into
 * filename
 */
GLuint save_png (const char * filename, GLubyte * pixels, GLuint w, GLuint h);

#endif /* _F_PNG_H */
