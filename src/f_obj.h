#ifndef _OBJ_H
#define _OBJ_H

typedef struct {
	char * tex;
	GLfloat shine;
	vec3 spec_col;
} material;

/*
 * Gets the texture, specular colour and specular exponent of a material from
 * the mtl file
 */ 
material read_mtl (const char * filename);

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices);

#endif /* _OBJ_H */
