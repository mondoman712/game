#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <libguile.h>

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices)
{
	int i;

	SCM vrts;

	/* load scheme source file */
	scm_c_primitive_load("src/f_obj.scm");

	/* load scheme packages to use */
	scm_c_use_module("ice-9 rdelim");
	scm_c_use_module("srfi srfi-1");

	/* load necessary scheme function */
	SCM load_obj_sym = scm_c_lookup("load-obj");
	SCM load_obj = scm_variable_ref(load_obj_sym);

	/* call scheme function */
	vrts = scm_call_1(load_obj, scm_from_locale_string(filename));

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

	return 0;
}
