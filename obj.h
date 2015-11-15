#ifndef _OBJ_H
#define _OBJ_H

/* 
 * Reads obj file and deposits vertices into float array
 */
int read_obj (const char * filename, GLfloat * vertices, GLuint * faces);

#endif /* _OBJ_H */
