#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
 * Float matrix math library.
 * 
 * Important features:
 * - COLUMN major format
 * - accepts angles as DEGREES
 * - matrix_rotate normalizes the arbitrary rotation axis (x, y, z) 
 * - matrices are 4x4
*/

// all matrices store enough floats for a 4x4
// (don't change this)
#define MATRIX_SIZE 16
#define MATRIX_WIDTH 4
#define MATRIX_HEIGHT 4

typedef struct{
	float data[MATRIX_SIZE];
} matrix;

float radians_to_degrees(float radians){
	return radians * (180.0f / M_PI);
}

float degrees_to_radians(float degrees){
	return (M_PI * degrees) / 180;
}

// Translates 2D coordinates to 1D index.
float matrix_get(matrix *m, size_t x, size_t y){
	return m->data[(MATRIX_WIDTH*y)+x];
}

void matrix_set(matrix *m, size_t x, size_t y, float v){
	m->data[(MATRIX_WIDTH*y)+x] = v;
}

void matrix_copy(matrix *dest, float *src){
	memcpy(dest->data, src, sizeof(float) * MATRIX_SIZE);
}

void matrix_identity(matrix *dest){
	matrix_copy(dest, (float[]){
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,	
		0.0f, 0.0f, 1.0f, 0.0f,	
		0.0f, 0.0f, 0.0f, 1.0f	
	});
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
		0.0f,       0.0f,          2.0f/(far-near), (far+near)/(far-near),
		//0.0f,       0.0f,          2.0f/(far-near), -(far+near)/(far-near),
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


// Arbitrary axis rotation
void matrix_rotate(
	matrix *dest, 
	float rx, float ry, float rz, 
	float angle
){
	float cosa, sina, a, x, y, z, m;
	
	a    = degrees_to_radians(angle);
	cosa = cosf(a);
	sina = sinf(a);

	// normalize the rotation axis
	m = sqrtf((rx * rx) + (ry * ry) + (rz * rz));
	x = rx / m;
	y = ry / m;
	z = rz / m;

	// If the vector is zero don't try to perform the rotation
	// otherwise you end up with a totally broken transformation.
	// Multiplying by identity is the equivalent of doing nothing.
	if( m == 0.0f )
		matrix_identity(dest);
	else
		matrix_copy(dest, (float[]){

		cosa+(x*x)*(1-cosa),   x*y*(1-cosa)-(z*sina), x*z*(1-cosa)+(y*sina), 0.0f,
		y*x*(1-cosa)+(z*sina), cosa+(y*y)*(1-cosa),   y*z*(1-cosa)-(x*sina), 0.0f,	
		z*x*(1-cosa)-(y*sina), z*y*(1-cosa)+(x*sina), cosa+(z*z)*(1-cosa),   0.0f,	
		0.0f,                  0.0f,                  0.0f,                  1.0f	
			
		});

}

void matrix_multiply(matrix *dest, matrix *a, matrix *b){
	size_t x, i, j;

	matrix tmp;

	for(i=0; i < MATRIX_HEIGHT; i++){
	for(j=0; j < MATRIX_WIDTH;  j++){
		float temp = 0;

		for(x=0; x < MATRIX_WIDTH;  x++)
			temp += matrix_get(a, x, i) * matrix_get(b, j, x);

		matrix_set(&tmp, j, i, temp);
	}
	}

	matrix_copy(dest, tmp.data);
}


// position  xyz
// angle     xyz
void matrix_camera(
	matrix *dest,
	float px, float py, float pz,
	float ax, float ay, float az
){

	matrix translate, rotate, temp;
	
	matrix_identity(&rotate);
	
	// build translation matrix
	matrix_translate(&translate, -px, -py, -pz);
	
	// build rotation matrix
	// (we don't use arbitrary rotation 
	// since we need specific values for each axis)
	matrix_rotate_x(&temp, ax); matrix_multiply(&rotate, &temp, &rotate);
	matrix_rotate_y(&temp, ay); matrix_multiply(&rotate, &temp, &rotate);
	matrix_rotate_z(&temp, az); matrix_multiply(&rotate, &temp, &rotate);

	// finnaly calculate the result into our 'destination' matrix
	matrix_multiply(dest, &rotate, &translate);
}

void matrix_print(matrix *m, int spacing, int precision){
	size_t x, y;

	// Custom formatting: we pre-format a format
	// string so we can get variable-spacing.
	char form[32];
	sprintf(form, "%%-%d.%df", spacing, precision);

	for(y=0; y < MATRIX_HEIGHT; y++){
		printf("%s", "| ");
		
		for(x=0; x < MATRIX_WIDTH; x++)
			printf(form, matrix_get(m, x, y));
		
		printf("%s", "|\n");
	}
	
	printf("%s", "\n");
}
#endif
