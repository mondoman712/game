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

static int load_obj (const char * filename)
{
	FILE * m = fopen(filename, "r");
	if (m == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}

	fclose(m);

	return 0;
}

int main ()
{
	float buff[3];

	parse_vector("v 0.437500 -0.765625 0.164062", buff);
	printf("%f, %f, %f\n", buff[0], buff[1], buff[2]);


	exit(EXIT_SUCCESS);
}
