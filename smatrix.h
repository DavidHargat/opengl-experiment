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

void matrix_scale(matrix *dest, float x, float y, float z){
	matrix_copy(dest, (float[]){
		   x, 0.0f, 0.0f, 0.0f,
		0.0f,    y, 0.0f, 0.0f,	
		0.0f, 0.0f,    z, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f	
	});
}

void matrix_translate(matrix *dest, float x, float y, float z){
	matrix_copy(dest, (float[]){
		1.0f, 0.0f, 0.0f,    x,
		0.0f, 1.0f, 0.0f,    y,	
		0.0f, 0.0f, 1.0f,    z,	
		0.0f, 0.0f, 0.0f, 1.0f	
	});
}


void matrix_perspective(
	matrix *dest, 
	float fovx, float fovy,
	float near, float far

){
	
	float fx, fy;

	fx = degrees_to_radians(fovx);
	fy = degrees_to_radians(fovy);

	matrix_copy(dest, (float[]){
		atan(fx/2.0f), 0.0f,          0.0f,                  0.0f,
		0.0f,          atan(fy/2.0f), 0.0f,                  0.0f,
		0.0f,          0.0f,         -(far+near)/(far-near), -(2.0f * far * near)/(far-near),
		0.0f,          0.0f,          -1.0f,                 1.0f
	});
}

/* matrix_orthographic
 *   The idea here is that we map some arbitrary
 *   size coordinate system to NDC.
 *   For example the coordinates:
 *      (5, 5, 5)
 *   in an orthographic system where: 
 *      {width=10 | height=10 | depth=10}
 *   become:
 *      (0.5, 0.5, 0.5)
 */
void matrix_orthographic(
	matrix *dest,
	float width, float height,
	float near,  float far
){
	matrix_copy(dest, (float[]){
		1.0f/width, 0.0f,          0.0f,            0.0f,
		0.0f,       1.0f/height,   0.0f,            0.0f,
		0.0f,       0.0f,          2.0f/(far-near), -(far+near)/(far-near),
		0.0f,       0.0f,          0.0f,            1.0f
	});
}

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
		 0.0f, 1.0f, 0.0f, 0.0f,	
		-asin, 0.0f, acos, 0.0f,	
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
		return;

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
