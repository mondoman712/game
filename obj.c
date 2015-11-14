#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

/* 
 * Puts the thee floats in the string str into an array given by buff
 */
static int parse_vector (char * str, GLfloat * buff)
{
	char * _str = malloc(sizeof(char) * 32);
	char * wd = malloc(sizeof(char) * 10);
	int i = 0;

	if (_str == NULL || wd == NULL) 
		goto cleanup;

	strcpy(_str, str);
	strtok(_str, " ");

	while ((wd = strtok(NULL, " "))) {
		buff[i] = atof(wd);
		i++;
	}

cleanup:
	free(_str);
	free(wd);

	return 0;
}

static int parse_face (char * str, GLuint * buff)
{
	char * _str = malloc(sizeof(char) * 35);
	char * wd = malloc(sizeof(char) * 12);
	int i = 0;

	if (_str == NULL || wd == NULL) 
		goto cleanup;

	strcpy(_str, str);
	strtok(_str, " ");

	while ((wd = strtok(NULL, " "))) {
		buff[i] = atoi(wd) - 1;
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
size_t read_obj (const char * filename, GLfloat * vertices, GLuint * faces,
		GLuint * facec)
{
	char buf[80];
	int vertc = 0;
	GLfloat * _verts = vertices;
	GLuint * _faces = faces;
	GLuint fc = 0;

	FILE * m = fopen(filename, "r");
	if (m == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}

	while (fgets(buf, sizeof(buf), m)) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			parse_vector(buf, _verts +(vertc * 3));
			vertc++;
		} else if (buf[0] == 'f') {
			parse_face(buf, _faces + (fc * 3));
			fc++;
		}

	}

	fclose(m);

	return vertc;
}

int main ()
{
	size_t vertlen = 0;
	GLuint * facec = 0;
	int i;

	GLfloat * vertices = malloc(128 * sizeof(GLfloat));
	GLuint * faces = malloc(128 * sizeof(GLuint));
	vertlen = read_obj("models/sphere.obj", vertices, faces, facec);

	printf("%i\n", (int) vertlen);

	for (i = 0; i < (int) (vertlen * 3); i += 3) {
		printf("%f, ", *(vertices + i));
		printf("%f, ", *(vertices + i + 1));
		printf("%f\n", *(vertices + i + 2));
	}

	exit(EXIT_SUCCESS);
}
