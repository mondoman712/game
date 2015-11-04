#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* 
 * Puts the thee floats in the string str into an array given by buff
 */
static int parse_vector (char * str, float * buff)
{
	return 0;
}

#ifdef FALSE
int load_obj (const char * filename)
{
	FILE * m = fopen(filename, "r");
	if (m == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}
	
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	size_t vert_size = 0;
	int i = 0;

	while ((read = getline(&line, &len, m)) != -1) {
		if (line[0] == 'v' && line[1] == ' ') {

		} else if (line[0] == 'f' && line[1] == ' ') {

		}
	}

	free(line);
	fclose(m);

	return 0;
}
#endif

int main ()
{
	float buff[3];

	parse_vector("v 0.437500 -0.765625 0.164062", buff);
	//printf("%f, %f, %f\n", buff[0], buff[1], buff[2]);

	exit(EXIT_SUCCESS);
}
