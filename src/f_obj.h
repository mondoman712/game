#ifndef _OBJ_H
#define _OBJ_H

/*
 * Finds the texture of an object in the mtl file, and returns the file name as
 * a string
 */
char * find_tex (const char * filename);

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices);

#endif /* _OBJ_H */
