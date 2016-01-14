#ifndef _TRANS_H
#define _TRANS_H

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3;

/*
 * Deposits the values in a vec3 into the first 3 places in an array
 */
void vec3_array (vec3 in, GLfloat * array);

/*
 * Defines a matrix transformation to move the camera position
 */
void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4);

/*
 * Defines a View matrix given an eye position, and pitch and yaw angles
 */
void look_to (vec3 eye, GLfloat pitch, GLfloat yaw, GLfloat * mat4);

/*
 * Defines a perspective projection matrix transformation
 */
void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4);

/*
 * Defines a matrix transformation to rotate around the x axis
 */
void rotatex (GLfloat ang, GLfloat * mat4);

/*
 * Defines a matrix transformation to rotate around the y axis
 */
void rotatey (GLfloat ang, GLfloat * mat4);

/*
 * Defines a matrix transformation to rotate around the Z axis
 */
void rotatez (GLfloat ang, GLfloat * mat4);

/*
 * Defines a matrix transformation for rotations in the x, y and z axes.
 */
void rotate (GLfloat x, GLfloat y, GLfloat z, GLfloat * mat4);

/*
 * Creates a matrix to move a vector by the vector pos
 */
void translate (vec3 pos, GLfloat * mat4);

/*
 * Defines a matrix transformation that moves a vector and rotates it
 */
void trans_rot (vec3 pos, vec3 rot, GLfloat *mat4);

#endif /* _TRANS_H */
