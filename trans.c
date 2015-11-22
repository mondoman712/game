#include <math.h>
#include <assert.h>

#include <GL/glew.h>

#include "trans.h"

/*
 * finds the modulus of a 1x3 vector
 */
GLfloat mod_vec3 (vec3 in)
{
	return sqrt((in.x * in.x) + (in.y  * in.y) + (in.z * in.z));
}

/*
 * Finds the normal of a 1x3 vector
 */
vec3 norm_vec3 (vec3 in)
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
vec3 cross_vec3 (vec3 a, vec3 b)
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

	*mat4 = s.x;
	*(mat4 + 1) = u.x;
	*(mat4 + 2) = -f.x;
	*(mat4 + 3) = 1;

	*(mat4 + 4) = s.y;
	*(mat4 + 5) = u.y;
	*(mat4 + 6) = -f.y;
	*(mat4 + 7) = 1;

	*(mat4 + 8) = s.z;
	*(mat4 + 9) = u.z;
	*(mat4 + 10) = -f.z;
	*(mat4 + 11) = 1;

	*(mat4 + 12) = - dot_vec3(s, eye);
	*(mat4 + 13) = - dot_vec3(u, eye);
	*(mat4 + 14) = dot_vec3(f, eye);
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
	*(mat4 + 4) = 0;
	*(mat4 + 8) = 0;
	*(mat4 + 12) = 0;

	*(mat4 + 1) = 0;
	*(mat4 + 5) = 1 / f;
	*(mat4 + 9) = 0;
	*(mat4 + 13) = 0;

	*(mat4 + 2) = 0;
	*(mat4 + 6) = 0;
	*(mat4 + 10) = - (zfar + znear) / (zfar - znear); 
	*(mat4 + 14) = - (2 * zfar * znear) / (zfar - znear); 

	*(mat4 + 3) = 0;
	*(mat4 + 7) = 0;
	*(mat4 + 11) = -1;
	*(mat4 + 15) = 0;
}

/*
 * Defines a matrix transformation to rotate around the Z axis
 */
void rotatez (GLfloat ang, GLfloat * mat4)
{
	*mat4 = cos(ang);
	*(mat4 + 1) = sin(ang);
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = -sin(ang);
	*(mat4 + 5) = cos(ang);
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = 1;
	*(mat4 + 11) = 0;

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1;
}

