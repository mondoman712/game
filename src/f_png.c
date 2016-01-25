#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <zlib.h>
#include <png.h>

#include "f_png.h"

#define PNG_SIGBYTES 8

/*
 * Checks the signature of the png file being read at stream
 */
static GLuint readpng_checksig (FILE * stream)
{
	GLubyte sig[PNG_SIGBYTES];
	fread(sig, 1, PNG_SIGBYTES, stream);
	return !png_check_sig(sig, PNG_SIGBYTES);
}

/*
 * Reads png file
 */
image read_png (const char * filename)
{
	image ret;
	ret.w = 0;
	ret.h = 0;
	ret.data = NULL;
	ret.colour_type = 0;

	/* Open File */
	FILE * fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		return ret;
	}

	/* Check Signature */
	if (readpng_checksig(fp)) {
		fprintf(stderr, "PNG signature for %s is invalid\n", filename);
		goto cleanup;
	}

	/* Create png structs */
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
			NULL, NULL);
	if (!png)
		goto cleanup;

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_read_struct(&png, (png_infopp) NULL,
				(png_infopp) NULL);
		goto cleanup;
	}

	png_infop end_info = png_create_info_struct(png);
	if (!end_info) {
		png_destroy_read_struct(&png, &info, (png_infopp) NULL);
		goto cleanup;
	}

	/* Set libpng error jump point */
	if (setjmp(png_jmpbuf(png))) {
		goto cleanup1;
	}

	/* Actually read the png */
	png_init_io(png, fp);
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);

	GLint bit_depth, act_colour_type;
	png_uint_32 tmpw, tmph;
	png_get_IHDR(png, info, &tmpw, &tmph, &bit_depth, &act_colour_type, NULL,
			NULL, NULL);

	ret.w = tmpw;
	ret.h = tmph;

	char * s;
	switch (act_colour_type) {
	case PNG_COLOR_TYPE_RGB:
		s = "RGB";
		ret.colour_type = GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		s = "RGBA";
		ret.colour_type = GL_RGBA;
		break;
	default:
		s = "Unknown";
		ret.colour_type = GL_RGB;
		break;
	}

	printf("%s:", filename);
	printf("\twidth = %i,\t height = %i", tmpw, tmph);
	printf("\tbit depth = %i,\t colour type = %s\n", bit_depth, s);

	png_read_update_info(png, info);
	size_t rowbytes = png_get_rowbytes(png, info);
	rowbytes += 3 - ((rowbytes - 1) % 4);

	png_byte * img_data = NULL;
	img_data = malloc(rowbytes * tmph * sizeof(png_byte) + 15);
	if (img_data == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		goto cleanup1;
	}

	png_bytep * row_ptrs = malloc(tmph * sizeof(png_bytep));
	if (row_ptrs == NULL) {
		fprintf(stderr, "Failed to allocate memory for %s\n", filename);
		goto cleanup1;
	}

	GLuint i;
	for (i = 0; i < tmph; i++)
		row_ptrs[tmph - 1 - i] = img_data + i * rowbytes;
			
	png_read_image(png, row_ptrs);

	ret.data = img_data;

	/* Free data and close file */
	free(row_ptrs);
cleanup1:
	png_destroy_read_struct(&png, &info, &end_info);
cleanup:
	fclose(fp);
	return ret;
}

/*
 * Saves a byte array stored in pixels of width w and height h as a png into
 * filename
 */
GLuint save_png (const char * filename, GLubyte * pixels, GLuint w, GLuint h)
{
	png_structp png;
	png_infop info;
	png_colorp palette;
	png_bytepp rows;
	FILE * fp;

	GLuint i;

	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) return 1;

	info = png_create_info_struct(png);
	if(!info) {
		png_destroy_write_struct(&png, NULL);
		return 1;
	}

	fp = fopen(filename, "wb");
	if (!fp) {
		png_destroy_write_struct(&png, &info);
		return 1;
	}

	png_init_io(png, fp);
	png_set_IHDR(png, info, w, h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	palette = (png_colorp) png_malloc(png, PNG_MAX_PALETTE_LENGTH
			* sizeof(png_color));
	if (!palette) {
		fclose(fp);
		png_destroy_write_struct(&png, &info);
		return 1;
	}

	png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
	png_write_info(png, info);
	png_set_packing(png);

	rows = (png_bytepp) png_malloc(png, h * sizeof(png_bytep));
	
	for (i = 0; i < h; ++i)
		rows[i] = (png_bytep) (pixels + (h - i - 1) * w * 3);

	png_write_image(png, rows);
	png_write_end(png, info);
	png_free(png, palette);
	png_destroy_write_struct(&png, &info);

	fclose(fp);
	free(rows);
	
	return 0;
}
