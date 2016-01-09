#ifndef _OBJ_H
#define _OBJ_H

/*
 * Gets the texture, specular colour and specular exponent of a material from
 * the mtl file
 */ 
char * read_mtl (const char * filename, vec3 * speccol, GLfloat * specexp);

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices);

#endif /* _OBJ_H */
