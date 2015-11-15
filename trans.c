#include <math.h>
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
 * Deposits the values in a vec3 into the first 3 places in an array
 */
void vec3_array (vec3 in, GLfloat * array)
{
	*(array) = in.x;
	*(array + 1) = in.y;
	*(array + 2) = in.z;
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
	up = norm_vec3(up);

	s = cross_vec3(f, up);
	u = cross_vec3(norm_vec3(s), f);

	vec3_array(s, mat4);
	*(mat4 + 3) = 0;
	vec3_array(u, (mat4 + 4));
	*(mat4 + 7) = 0;

	f.x = -f.x;
	f.y = -f.y;
	f.z = -f.z;

	vec3_array(f, (mat4 + 8));
	*(mat4 + 11) = 0;
	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1;
}

#ifdef FALSE
GLfloat dot_vec3 (vec3 a, vec3 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void look_at (vec3 eye, vec3 centre, vec3 up, GLfloat * mat4)
{
	vec3 x, y, z;

	z.x = centre.x - eye.x;
	z.y = centre.y - eye.y;
	z.z = centre.z - eye.z;
	z = norm_vec3(z);

	y = up;
	x = cross_vec3(y, z);
	y = cross_vec3(z, x);

	y = norm_vec3(y);
	z = norm_vec3(z);

	vec3_array(x, mat4);
	*(mat4 + 3) = - dot_vec3(x, eye);

	vec3_array(y, (mat4 + 4));
	*(mat4 + 7) = - dot_vec3(y, eye);

	vec3_array(z, (mat4 + 8));
	*(mat4 + 11) = - dot_vec3(z, eye);

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = 0;
	*(mat4 + 15) = 1.0;
}
#endif

/*
 * Defines a perspective projection matrix transformation
 */
void perspective (GLfloat fovy, GLfloat asp, GLfloat znear, GLfloat zfar,
		GLfloat * mat4)
{
	GLfloat f = 1 / tan(fovy / 2);
	*mat4 = f / asp;
	*(mat4 + 1) = 0;
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = 0;
	*(mat4 + 5) = f;
	*(mat4 + 6) = 0;
	*(mat4 + 7) = 0;

	*(mat4 + 8) = 0;
	*(mat4 + 9) = 0;
	*(mat4 + 10) = (zfar + znear) / (zfar - znear); 
	*(mat4 + 11) = (2 * zfar * znear) / (zfar - znear); 

	*(mat4 + 12) = 0;
	*(mat4 + 13) = 0;
	*(mat4 + 14) = -1;
	*(mat4 + 15) = 0;
}

/*
 * Defines a matrix transformation to rotate around the Z axis
 */
void rotatez (GLfloat ang, GLfloat * mat4)
{
	*mat4 = cos(ang);
	*(mat4 + 1) = -sin(ang);
	*(mat4 + 2) = 0;
	*(mat4 + 3) = 0;

	*(mat4 + 4) = sin(ang);
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

