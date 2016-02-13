#ifndef _OBJ_H
#define _OBJ_H

typedef struct {
	GLint tex;
	GLfloat shine;
	vec3 spec_col;
	GLint illum;
} material;

typedef struct {
	GLuint vbo;
	vec3 pos;
	vec3 rot;
	material mat;
	GLfloat nverts;
} object;

/*
 * Gets the texture, specular colour and specular exponent of a material from
 * the mtl file
 */ 
material read_mtl (const char * filename, GLuint shader_prog);

/* 
 * Reads obj file and deposits vertices into float array
 */
GLuint read_obj (const char * filename, GLfloat ** vertices, char ** mtl_loc);

/*
 * Builds an object struct and loads the information needed from files
 */
object build_obj (const char * name, GLuint shader_prog);

#endif /* _OBJ_H */
