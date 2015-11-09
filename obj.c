#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Puts the thee floats in the string str into an array given by buff */
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

/* Reads obj file and deposits vertices into float array */
size_t read_obj (const char * filename, float * vertices)
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
			vertc++;
		}
	}

	fclose(m);

	return vertc;
}

#ifdef FALSE
int main ()
{
	size_t vertlen = 0;
	int i;

	float * vertices = malloc(128 * sizeof(float));
	vertlen = read_obj("models/sphere.obj", vertices);

	printf("%i\n", vertlen);

	for (i = 0; i < (int) vertlen; i++)
		printf("%f, %f, %f\n", *vertices++, *vertices++, *vertices++);

	exit(EXIT_SUCCESS);
}
#endif
