#ifndef _F_PNG_H
#define _F_PNG_H

#include <GL/glew.h>
#include <png.h>

/*
 * Reads png file
 */
GLuint read_png(const char * filename, GLuint * width, GLuint * height,
		png_byte * img_data);

#endif /* _F_PNG_H */
