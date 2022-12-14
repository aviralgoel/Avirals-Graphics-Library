#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include <algorithm>

GzColor* image = NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char		pixel[3];
	unsigned char     dummy;
	char  		foo[8];
	int   		i, j;
	FILE* fd;

	if (reset) {          /* open and load texture file */
		fd = fopen("texture", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor) * (xs + 1) * (ys + 1));
		if (image == NULL) {
			fprintf(stderr, "mallolength % 10==0for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs * ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	if (u < 0) {
		u = 0; // Clamp
	}
	if (u > 1.0f) {
		u = 1.0f; // Clamp
	}
	if (v < 0) {
		v = 0; // Clamp
	}
	if (v > 1.0f) {
		v = 1.0f; // Clamp
	}

	u *= (xs - 1);
	v *= (ys - 1);

	/* determine texture cell corner values and perform bilinear interpolation */
	int  x1 = (int)floor(u), x2 = (int)ceil(u), y1 = (int)floor(v), y2 = (int)ceil(v);

	float s = u - x1, t = v - y1;

	/* set color to interpolated GzColor value and return */
	float A[] = { image[y1 * xs + x1][0],image[y1 * xs + x1][1],image[y1 * xs + x1][2] };
	float B[] = { image[y1 * xs + x2][0],image[y1 * xs + x2][1],image[y1 * xs + x2][2] };
	float C[] = { image[y2 * xs + x2][0],image[y2 * xs + x2][1],image[y2 * xs + x2][2] };
	float D[] = { image[y2 * xs + x1][0],image[y2 * xs + x1][1],image[y2 * xs + x1][2] };

	for (int i = 0; i < 3; i++) {
		color[i] = s * t * C[i] + (1.0f - s) * t * D[i] + s * (1.0f - t) * B[i] + (1.0f - s) * (1.0f - t) * A[i];
	}

	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	xs = 200;
	ys = 200;

	int quadSize = 20;
	if (u > 1.0f)
		u = 1.0f;
	if (u < 0)
		u = 0;
	if (v > 1.0f)
		v = 1.0f;
	if (v < 0)
		v = 0;

	
	float s = u * 0.93 - v * 0.7;
	float t = v * 0.7 + u * 0.93;
	float scaleS = 8, scaleT = 2;
	float pattern = (((s * scaleS) < 0.5) ^ ((t * scaleT) < 0.5));
	if (pattern) {
		color[RED] = 0.9f;
		color[GREEN] = 0.4f;
		color[BLUE] = 0.1f;
	}
	else {
		color[RED] = 0.1f;
		color[GREEN] = 0.9f;
		color[BLUE] = 0.9f;
	}

	return GZ_SUCCESS;
}


/* Free texture memory */
int GzFreeTexture()
{
	if (image != NULL)
		free(image);
	return GZ_SUCCESS;
}
