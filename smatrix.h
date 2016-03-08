#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Allocate a matrix on the stack.
#define matrix_salloc(dest, w, h) \
dest.width  = w;                  \
dest.height = h;                  \
dest.data   = (float[(w*h)]){};   \

typedef struct{
	size_t width;
	size_t height;
	float *data;
} matrix;

float radians_to_degrees(float radians){
	return radians * (180.0f / M_PI);
}

float degrees_to_radians(float degrees){
	return (M_PI * degrees) / 180;
}

// Translates 2D coordinates to 1D index.
float matrix_get(matrix *m, size_t x, size_t y){
	return m->data[(m->width*y)+x];
}

void matrix_set(matrix *m, size_t x, size_t y, float v){
	m->data[(m->width*y)+x] = v;
}

void matrix_copy(matrix *dest, float *src){
	memcpy(dest->data, src, sizeof(float) * (dest->width) * (dest->height));
}


//matrix *matrix_

/* The following functions build
 * matrices for 3D vector rotation.
 * For example, rotation in the X plane,
 * where a is the angle:
 * | 1    0       0       0 |
 * | 0    cos(a) -sin(a)  0 |
 * | 0    sin(a)  cos(a)  0 |
 * | 0    0       0       1 |
 */
void matrix_rotate_x(matrix *dest, float angle){
	float acos, asin, a;

	a    = degrees_to_radians(angle);
	acos = cosf(a);
	asin = sinf(a);
	
	matrix_copy(dest, (float[]){
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, acos,-asin, 0.0f,	
		0.0f, asin, acos, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f	
	});
}

void matrix_rotate_y(matrix *dest, float angle){
	float acos, asin, a;
	
	a    = degrees_to_radians(angle);
	acos = cosf(a);
	asin = sinf(a);
	
	matrix_copy(dest, (float[]){
		 acos, 0.0f, asin, 0.0f,
		 0.0f, 1.0f,    0, 0.0f,	
		-asin, 0,    acos, 0.0f,	
		 0.0f, 0.0f, 0.0f, 1.0f	
	});
}

void matrix_rotate_z(matrix *dest, float angle){
	float acos, asin, a;
	
	a    = degrees_to_radians(angle);
	acos = cosf(a);
	asin = sinf(a);
	
	matrix_copy(dest, (float[]){
		acos,-asin, 0.0f, 0.0f,
		asin, acos, 0.0f, 0.0f,	
		0.0f, 0.0f, 1.0f, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f	
	});
}

void matrix_multiply(matrix *dest, matrix *a, matrix *b){
	size_t x, i, j;

	// The size of the ROWS in matrix A
	// must be the same as the size of the
	// COLUMNS in matrix B.
	if(a->width != b->height)
		return NULL;

	for(i=0; i < a->height; i++){
	for(j=0; j < b->width;  j++){
		float temp = 0;

		for(x=0; x < a->width;  x++)
			temp += matrix_get(a, x, i) * matrix_get(b, j, x);

		matrix_set(dest, j, i, temp);
	}
	}
}

void matrix_print(matrix *m, int spacing, int precision){
	size_t x, y;

	// Custom formatting: we pre-format a format
	// string so we can get variable-spacing.
	char form[32];
	sprintf(form, "%%-%d.%df", spacing, precision);

	for(y=0; y < (m->height); y++){
		printf("%s", "| ");
		
		for(x=0; x < (m->width); x++)
			printf(form, matrix_get(m, x, y));
		
		printf("%s", "|\n");
	}
	
	printf("%s", "\n");
}
#endif
