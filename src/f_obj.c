#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <libguile.h>

/* 
 * Puts the three floats in the string str into an array given by buff
 */
static GLushort parse_vector (char * str, GLfloat * buff, SCM fun)
{
	SCM str_numlist_sym;
	SCM str_numlist;
	SCM numlist;
	int i;

	numlist = scm_call_1(fun, scm_from_locale_stringn(str, strlen(str)));
	
	printf("%i\n", scm_to_int(scm_length(numlist)));
	printf("%f, ", scm_to_double(scm_list_ref(numlist, scm_from_int(0))));
	printf("%f, ", scm_to_double(scm_list_ref(numlist, scm_from_int(1))));
	printf("%f\n", scm_to_double(scm_list_ref(numlist, scm_from_int(2))));

	return 0;
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
	FILE * fp;

	SCM verts;
	scm_c_primitive_load("src/f_obj.scm");
	scm_c_use_module("ice-9 rdelim");
	SCM load_obj_sym = scm_c_lookup("load-obj");
	SCM load_obj = scm_variable_ref(load_obj_sym);

	int i;
	verts = scm_call_1(load_obj, scm_from_locale_string(filename));
	*vertices = scm_to_int(scm_length(verts));
	for (i = 1; i < *vertices; i++) {
		*(vertices + i) =  (GLfloat) scm_to_double(scm_list_ref(verts,
					scm_from_int(i)));
	}

	/*
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}
	
	while (fgets(buf, sizeof(buf), fp)) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			parse_vector(buf, vertices + (vc * 3) + 1, str_numlist);
			vc++;
		} else if (buf[0] == 'f' && buf[1] == ' ') {
			parse_face(buf, faces + (fc * 3) + 1);
			fc++;
		}
	}

	fclose(fp);

	*vertices = (GLfloat) vc;
	*faces = fc;
	*/

	return 0;
}
