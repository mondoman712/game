#ifndef _TRANS_H
#define _TRANS_H

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3;

/*
 * finds the modulus of a 1x3 vector
 */
GLfloat mod_vec3 (vec3 in);

/*
 * Finds the normal of a 1x3 vector
 */
vec3 norm_vec3 (vec3 in);

/*
 * Finds the cross product of vectors a and b
 */
vec3 cross_vec3 (vec3 a, vec3 b);

/*
 * Deposits the values in a vec3 into the first 3 places in an array
 */
void vec3_array (vec3 in, GLfloat * array);

/*
 * Defines a matrix transformation to move the camera position
 */
void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4);

/*
 * Defines a perspective projection matrix transformation
 */
void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4);

/*
 * Defines a matrix transformation to rotate around the Z axis
 */
void rotatez (GLfloat ang, GLfloat * mat4);
#endif /* _TRANS_H */
