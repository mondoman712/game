#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <libguile.h>

/* 
 * Puts the three floats in the string str into an array given by buff
 */
static GLushort parse_vector (char * str, GLfloat * buff)
{
}

/*
 * Puts the three ints in the string str into an array given by buff
 *
 * TODO: Merge this and parse_obj because of large amounts of copied code
 */
static GLushort parse_face (char * str, GLuint * buff)
{
	GLchar * _str = malloc(sizeof(GLchar) * 35);
	GLchar * wd = malloc(sizeof(GLchar) * 12);
	GLushort i = 0;

	if (_str == NULL || wd == NULL) 
		goto cleanup;

	strcpy(_str, str);
	strtok(_str, " ");

	while ((wd = strtok(NULL, " "))) {
		buff[i] = (GLuint) (atoi(wd) - 1);
		i++;
	}

cleanup:
	free(_str);
	free(wd);

	return 0;
}

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat * vertices, GLuint * faces)
{
	char buf[64];
	GLushort vc = 0;
	GLushort fc = 0;

	FILE * m = fopen(filename, "r");
	if (m == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}
	
	while (fgets(buf, sizeof(buf), m)) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			parse_vector(buf, vertices + (vc * 3) + 1);
			vc++;
		} else if (buf[0] == 'f' && buf[1] == ' ') {
			parse_face(buf, faces + (fc * 3) + 1);
			fc++;
		}
	}

	fclose(m);

	*vertices = (GLfloat) vc;
	*faces = fc;

	return 0;
}
