#ifndef _PNG_H
#define _PNG_H
/*
 * Reads png file
 */
GLuint read_png(const char * filename, GLuint * width, GLuint * height,
		png_byte * img_data);

#endif /* _PNG_H */
