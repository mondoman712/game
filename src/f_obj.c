#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <libguile.h>

#include "slibs/trans.h"
#include "f_obj.h"
#include "slibs/f_png.h"

#define MTLBUFFSIZE 64 * sizeof(char)

#define MODEL_DIR "assets/models/"
#define MODEL_EXT ".obj"
#define MTL_EXT ".mtl"

/*
 * Gets the texture, specular colour and specular exponent of a material from
 * the mtl file
 */ 
material read_mtl (const char * filename, GLuint shader_prog)
{
	char * buff = malloc(MTLBUFFSIZE);
	char * texloc = malloc(MTLBUFFSIZE);
	FILE * fp;

	material ret = {0, 0, (vec3) {0, 0, 0}, 0};

	if (buff == NULL) {
		fprintf(stderr, "Failed to allocate memory for: %s\n", filename);
		return ret;
	}

 	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return ret;
	}

	while (fgets(buff, MTLBUFFSIZE, fp)) {
		if (buff[4] == 'K' && buff[5] == 'd') {
			texloc = malloc(1 + strlen(buff + 7));
			strcpy(texloc, buff + 7);
			*(texloc + strlen(texloc) - 1) = '\0';
		} else if (buff[0] == 'K' && buff[1] == 's') {
			strtok(buff, " ");
			ret.spec_col.x = atof(strtok(NULL, " "));
			ret.spec_col.y = atof(strtok(NULL, " "));
			ret.spec_col.z = atof(strtok(NULL, " "));
		} else if (buff[0] == 'N' && buff[1] == 's') {
			strtok(buff, " ");
			ret.shine = atof(strtok(NULL, " "));
		} else if (buff[0] == 'i' && buff[4] == 'm') {
			strtok(buff, " ");
			ret.illum = atoi(strtok(NULL, " "));
		}
	}

	GLuint tex;
	glGenTextures(1, &tex);
	image img;

	char * fulltexloc = malloc(1 + strlen(texloc) + strlen(MODEL_DIR));
	strcpy(fulltexloc, MODEL_DIR);
	strcat(fulltexloc, texloc);

	glBindTexture(GL_TEXTURE_2D, tex);
	img = read_png(fulltexloc);
	glTexImage2D(GL_TEXTURE_2D, 0, img.colour_type, img.w, img.h, 0,
			img.colour_type, GL_UNSIGNED_BYTE, img.data);
	glUniform1i(glGetUniformLocation(shader_prog, "tex"), 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	ret.tex = tex;

	free(buff);
	free(texloc);
	free(fulltexloc);
	free(img.data);

	fclose(fp);

	return ret;
}

/* 
 * Reads obj file and deposits vertices into float array
 */
GLushort read_obj (const char * filename, GLfloat ** vertices, char ** mtl_loc)
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

	*mtl_loc = scm_to_locale_string(scm_list_ref(vrts, scm_from_int(1)));
	vrts = scm_list_ref(vrts, scm_from_int(0));

	/* Copy vertices from scm list into array */
	*vertices = (GLfloat *) malloc((1 + scm_to_int(scm_length(vrts))) 
			* sizeof(GLfloat));
	if (*vertices == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		return 1;
	}

	**vertices = (GLfloat) scm_to_double(scm_length(vrts));
	for (i = 0; i < **vertices; i++)
		*(*vertices + i + 1) =
			(GLfloat) scm_to_double(scm_list_ref(vrts,
						scm_from_int(i)));

	return 0;
}

/*
 * Builds an object struct and loads the information needed from files
 */
object build_obj (const char * name, GLuint shader_prog)
{
	object ret;
	ret.pos = (vec3) {0.0, 0.0, 0.0};
	ret.rot = (vec3) {0.0, 0.0, 0.0};

	char * objloc = malloc(1 + strlen(name) + strlen(MODEL_DIR)
			+ strlen(MODEL_EXT));
	strcpy(objloc, MODEL_DIR);
	strcat(objloc, name);
	strcat(objloc, MODEL_EXT);

	char * mtl = NULL;
	read_obj(objloc, &ret.verts, &mtl); 
	free(objloc);

	char * mtlloc = malloc(1 + strlen(mtl) + strlen(MODEL_DIR));
	strcpy(mtlloc, MODEL_DIR);
	strcat(mtlloc, mtl);

	ret.mat = read_mtl(mtlloc, shader_prog);
	free(mtlloc);

	glGenBuffers(1, &ret.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vbo);
	glBufferData(GL_ARRAY_BUFFER, (int) (*ret.verts * sizeof(GLfloat)),
			ret.verts + 1, GL_STATIC_DRAW);	

	return ret;
}
