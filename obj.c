#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* 
 * Puts the thee floats in the string str into an array given by buff
 */
static int parse_vector (char * str, float * buff)
{
	char * _str = malloc(sizeof(char) * 80);
	int i = 0;
	char * wd = malloc(sizeof(char) * 20);

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

static int load_obj (const char * filename, float * vertices, size_t * vertlen)
{
	char buf[80];
	int vertc = 0;

	FILE * m = fopen(filename, "r");
	if (m == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}

	while (fgets(buf, sizeof(buf), m)) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			parse_vector(buf, vertices);
			*vertices += 3;
			vertc += 3;
		}
	}

	fclose(m);

	printf("%i\n", vertc);

	vertlen += vertc;

	return 0;
}

int main ()
{
	float buff[3];
	size_t * vertlen = 0;

	float * vertices = malloc(64 * sizeof(float));
	load_obj("models/sphere.obj", vertices, vertlen);

	printf("%i\n", vertlen);

	exit(EXIT_SUCCESS);
}
