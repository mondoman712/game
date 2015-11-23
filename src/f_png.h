#ifndef _F_PNG_H
#define _F_PNG_H

#include <GL/glew.h>
#include <png.h>

/*
 * Reads png file
 */
png_byte * read_png(const char * filename, GLuint * width, GLuint * height);

#endif /* _F_PNG_H */
