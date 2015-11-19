#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

/* 
 * Puts the three floats in the string str into an array given by buff
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
		buff[i] = (GLfloat) atof(wd);
		i++;
	}

cleanup:
	free(_str);
	free(wd);

	return 0;
}

/*
 * Puts the three ints in the string str into an array given by buff
 *
 * TODO: Merge this and parse_obj because of large amounts of copied code
 */
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
int read_obj (const char * filename, GLfloat * vertices, GLuint * faces)
{
	char buf[64];
	GLuint vc = 0;
	GLuint fc = 0;

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

#ifdef FALSE
int main ()
{
	int i;

	GLfloat * vertices = malloc(128 * sizeof(GLfloat));
	GLuint * faces = malloc(128 * sizeof(GLuint));
	if (vertices == NULL || faces == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		exit(EXIT_FAILURE);
	}
	read_obj("models/cube.obj", vertices, faces);

	printf("vertices: %i\n", (int) *vertices);
	printf("faces: %d\n", *faces);

	for (i = 1; i < (int) (*vertices * 3); i += 3) {
		printf("%f, ", *(vertices + i));
		printf("%f, ", *(vertices + i + 1));
		printf("%f\n", *(vertices + i + 2));
	}

	for (i = 1; i < (int) (*faces * 3); i += 3) {
		printf("%d, ", *(faces + i));
		printf("%d, ", *(faces + i + 1));
		printf("%d\n", *(faces + i + 2));
	}

	free(vertices);
	free(faces);

	exit(EXIT_SUCCESS);
}
#endif
