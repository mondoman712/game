#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <libguile.h>

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices, GLuint ** faces)
{
	int i;

	SCM res, vrts, fces;
	scm_c_primitive_load("src/f_obj.scm");
	scm_c_use_module("ice-9 rdelim");
	SCM load_obj_sym = scm_c_lookup("load-obj");
	SCM load_obj = scm_variable_ref(load_obj_sym);

	res = scm_call_1(load_obj, scm_from_locale_string(filename));
	vrts = scm_list_ref(res, scm_from_int(0));
	fces = scm_list_ref(res, scm_from_int(1));

	/* Copy vertices from scm list into array */
	*vertices = (GLfloat *) malloc((1 + scm_to_int(scm_length(vrts))) 
			* sizeof(GLfloat));
	if (*vertices == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		return 1;
	}
	**vertices = (GLfloat) scm_to_double(scm_length(vrts));
	for (i = 0; i < **vertices; i++)
		*(*vertices + i + 1) =  (GLfloat) scm_to_double(scm_list_ref(vrts,
					scm_from_int(i)));

	/* Copy faces from scm list into array */
	*faces = (GLuint *) malloc((1 + scm_to_int(scm_length(fces))) 
			* sizeof(GLuint));
	if (*faces == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		return 1;
	}
	**faces = (GLuint) scm_to_uint(scm_length(fces));
	for (i = 0; i < (int) **faces; i++)
		*(*faces + i + 1) =  (GLuint) scm_to_uint(scm_list_ref(fces,
					scm_from_int(i)));

	return 0;
}
