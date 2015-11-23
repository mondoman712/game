#ifndef _F_PNG_H
#define _F_PNG_H

#include <GL/glew.h>
#include <png.h>

/*
 * Reads png file
 */
png_byte * read_png(const char * filename, GLuint * width, GLuint * height);

/*
 * Saves a byte array stored in pixels of width w and height h as a png into
 * filename
 */
GLuint save_png (const char * filename, GLubyte * pixels, GLuint w, GLuint h);

#endif /* _F_PNG_H */
