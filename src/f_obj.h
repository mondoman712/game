#ifndef _OBJ_H
#define _OBJ_H

typedef struct {
	GLint tex;
	GLfloat shine;
	vec3 spec_col;
} material;

typedef struct {
	material mat;
	GLfloat * verts;
} object;

/*
 * Gets the texture, specular colour and specular exponent of a material from
 * the mtl file
 */ 
material read_mtl (const char * filename, GLuint shader_prog);

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices);

object build_obj (const char * name, GLuint shader_prog);

#endif /* _OBJ_H */
