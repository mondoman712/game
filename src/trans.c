/*
 * This file includes functions to create matrix transformations for
 * transforming 3D objects.
 *
 * They take a GLfloat matrix of size 16, and create matrices in this format:
 *
 * 0 	4 	8 	12
 * 1 	5 	9 	13
 * 2 	6 	10 	14
 * 3 	7 	11 	15
 */

#include <math.h>
#include <assert.h>

#include <GL/glew.h>

#include "trans.h"

/*
 * finds the modulus of a 1x3 vector
 */
static GLfloat mod_vec3 (vec3 in)
{
	return sqrt((in.x * in.x) + (in.y  * in.y) + (in.z * in.z));
}

/*
 * Finds the normal of a 1x3 vector
 */
static vec3 norm_vec3 (vec3 in)
{
	GLfloat mod = mod_vec3(in);
	vec3 ret = {
		in.x / mod,
		in.y / mod,
		in.z / mod
	};

	return ret;
}

/*
 * Finds the cross product of vectors a and b
 */
static vec3 cross_vec3 (vec3 a, vec3 b)
{
	vec3 ret;

	ret.x = (a.y * b.z) - (a.z * b.y);
	ret.y = (a.z * b.x) - (a.x * b.z);
	ret.z = (a.x * b.y) - (a.y * b.x);

	return ret;
}

/*
 * Calculates the dot product between two 3 value vectors
 */
static GLfloat dot_vec3 (vec3 a, vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/*
 * Fills a mat4 with zeros
 */
void zeros (GLfloat * mat4)
{
	GLushort i;
	for (i = 0; i < 16; i++)
		*(mat4 + i) = 0;
}

/*
 * Creates a 4x4 identity matrix
 */
void identity (GLfloat * mat4)
{
	zeros(mat4);
	
	*mat4 = 1;
	*(mat4 + 5) = 1;
	*(mat4 + 10) = 1;
	*(mat4 + 15) = 1;
}

/*
 * Defines a matrix transformation to move the camera position
 */
void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4)
{
	vec3 f, s, u;

	f.x = centre.x - eye.x;
	f.y = centre.y - eye.y;
	f.z = centre.z - eye.z;

	f = norm_vec3(f);
	s = norm_vec3(cross_vec3(f, up));
	u = cross_vec3(s, f);

	/*
	 * s.x 		s.y 		s.z 		0
	 * u.x 		u.y 		u.z 		0
	 * -f.x 	-f.y 		-f.z 		0
	 * 0 		0 		0 		1
	 */

	*mat4 = s.x;
	*(mat4 + 1) = u.x;
	*(mat4 + 2) = -f.x;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = s.y;
	*(mat4 + 5) = u.y;
	*(mat4 + 6) = -f.y;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = s.z;
	*(mat4 + 9) = u.z;
	*(mat4 + 10) = -f.z;
	*(mat4 + 11) = 0;

	*(mat4 + 12) = - dot_vec3(s, eye);
	*(mat4 + 13) = - dot_vec3(u, eye);
	*(mat4 + 14) = dot_vec3(f, eye);
	*(mat4 + 15) = 1;
}

/*
 * Defines a View matrix given an eye position, and pitch and yaw angles
 */
void look_to (vec3 eye, GLfloat pitch, GLfloat yaw, GLfloat * mat4)
{
	GLfloat cp = cos(pitch);
	GLfloat sp = sin(pitch);
	GLfloat cy = cos(yaw);
	GLfloat sy = sin(yaw);

	vec3 x = {cy, 0, -sy};
	vec3 y = {sy * sp, cp, cy * sp};
	vec3 z = {sy * cp, -sp, cp * cy};

	*mat4 = x.x;
	*(mat4 + 1) = y.x;
	*(mat4 + 2) = z.x;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = x.y;
	*(mat4 + 5) = y.y;
	*(mat4 + 6) = z.y;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = x.z;
	*(mat4 + 9) = y.z;
	*(mat4 + 10) = z.z;
	*(mat4 + 11) = 0;

	*(mat4 + 12) = - dot_vec3(x, eye);
	*(mat4 + 13) = - dot_vec3(y, eye);
	*(mat4 + 14) = - dot_vec3(z, eye);
	*(mat4 + 15) = 1;
}

/*
 * Defines a perspective projection matrix transformation
 */
void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4)
{
	assert(asp != 0);
	assert(znear != zfar);

	GLfloat f = tan(fovy / 2);
	*mat4 = 1 / (f * asp);
	*(mat4 + 1) = 0;
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = 0;
	*(mat4 + 5) = 1 / f;
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = - (zfar + znear) / (zfar - znear); 
	*(mat4 + 11) = -1;

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = - (2 * zfar * znear) / (zfar - znear); 
	*(mat4 + 15) = 0;
}

/*
 * Defines a matrix transformation to rotate around the x axis
 */
void rotatex (GLfloat ang, GLfloat * mat4)
{
	/*
	 * 1 		0 		0 		0
	 * 0 		cos(ang) 	-sin(ang) 	0
	 * 0 		sin(ang) 	cos(ang) 	0
	 * 0 		0 		0 		1
	 */
	identity(mat4);

	*(mat4 + 5) = cos(ang);
	*(mat4 + 6) = sin(ang);

	*(mat4 + 9) = -sin(ang);
	*(mat4 + 10) = cos(ang);
}

/*
 * Defines a matrix transformation to rotate around the y axis
 */
void rotatey (GLfloat ang, GLfloat * mat4)
{
	/*
	 * cos(ang)	0		sin(ang) 	0
	 * 0 		1 		0 		0
	 * -sin(ang) 	0 		cos(ang)	0
	 * 0 		0 		0 		1
	 */
	identity(mat4);

	*mat4 = cos(ang);
	*(mat4 + 2) = -sin(ang);

	*(mat4 + 8) = sin(ang);
	*(mat4 + 10) = cos(ang);
}

/*
 * Defines a matrix transformation to rotate around the Z axis
 */
void rotatez (GLfloat ang, GLfloat * mat4)
{
	/*
	 * cos(ang)	-sin(ang)	0 	 	0
	 * sin(ang) 	cos(ang)	0 		0
	 * 0 		0 		1 		0
	 * 0 		0 		0 		1
	 */
	identity(mat4);

	*mat4 = cos(ang);
	*(mat4 + 1) = sin(ang);

	*(mat4 + 4) = -sin(ang);
	*(mat4 + 5) = cos(ang);
}

/*
 * Defines a matrix transformation for rotations in the x, y and z axes.
 */
void rotate (GLfloat x, GLfloat y, GLfloat z, GLfloat * mat4)
{
	*mat4 = cos(y) * cos(z);
	*(mat4 + 1) = cos(z) * sin(x) * sin(y) + cos(x) * sin(z);
	*(mat4 + 2) = sin(x) * sin(z) - cos(x) * cos(z) * sin(y);
	*(mat4 + 3) = 0;

	*(mat4 + 4) = -cos(y) * sin(z);
	*(mat4 + 5) = cos(x) * cos(z) - sin(x) * sin(y) * sin(z);
	*(mat4 + 6) = cos(z) * sin(x) + cos(x) * sin(y) * sin(z);
	*(mat4 + 7) = 0;

	*(mat4 + 8) = sin(y);
	*(mat4 + 9) = -cos(y) * sin(x);
	*(mat4 + 10) = cos(x) * cos(y);
	*(mat4 + 11) = 0;

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1;
}

/*
 * Creates a matrix to move a vector by the vector pos
 */
void translate (vec3 pos, GLfloat * mat4)
{
	identity(mat4);

	*(mat4 + 12) = pos.x;
	*(mat4 + 13) = pos.y;
	*(mat4 + 14) = pos.z;
}

/*
 * Defines a matrix transformation that moves a vector and rotates it
 */
void trans_rot (vec3 pos, vec3 rot, GLfloat *mat4)
{
	*mat4 = cos(rot.y) * cos(rot.z);
	*(mat4 + 1) = cos(rot.z) * sin(rot.x) * sin(rot.y) + cos(rot.x)
		* sin(rot.z);
	*(mat4 + 2) = sin(rot.x) * sin(rot.z) - cos(rot.x) * cos(rot.z)
		* sin(rot.y);
	*(mat4 + 3) = 0;

	*(mat4 + 4) = -cos(rot.y) * sin(rot.z);
	*(mat4 + 5) = cos(rot.x) * cos(rot.z) - sin(rot.x) * sin(rot.y)
		* sin(rot.z);
	*(mat4 + 6) = cos(rot.z) * sin(rot.x) + cos(rot.x) * sin(rot.y)
		* sin(rot.z);
	*(mat4 + 7) = 0;

	*(mat4 + 8) = sin(rot.y);
	*(mat4 + 9) = -cos(rot.y) * sin(rot.x);
	*(mat4 + 10) = cos(rot.x) * cos(rot.y);
	*(mat4 + 11) = 0;

	*(mat4 + 12) = pos.x;
	*(mat4 + 13) = pos.y;
	*(mat4 + 14) = pos.z;
	*(mat4 + 15) = 1;
}
