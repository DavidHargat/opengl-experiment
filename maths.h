#ifndef MATHS_H
#define MATHS_H

#include "matrix.h"
#include "vec3.h"

/*
 * More math definitions,
 * depending on both matrix.h
 * and vec3.h
 */

void matrix_lookat(
	matrix *dest,
	float ex, float ey, float ez,
	float tx, float ty, float tz,
	float ux, float uy, float uz
){
	matrix M, T;
	vec3 eye, tar, up, f, s, u;
	
	eye = vec3_(ex, ey, ez);
	tar = vec3_(tx, ty, tz);
	up  = vec3_(ux, uy, uz);
	f   = vec3_zero();
	s   = vec3_zero();
	u   = vec3_zero();

	vec3_subtract(f, tar, eye);
	vec3_normalize(f, f);
	vec3_normalize(up, up);
	
	vec3_cross(s, f, up);
	vec3_cross(u, s, f);

	matrix_copy(&M, (float[]){
		 s[0], s[1], s[2], 0.0f,
		 u[0], u[1], u[2], 0.0f,
		-f[0],-f[1],-f[2], 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f,
	});
	
	matrix_copy(&T, (float[]){
		 1.0f, 0.0f, 0.0f,-eye[0],
		 0.0f, 1.0f, 0.0f,-eye[1],
		 0.0f, 0.0f, 1.0f,-eye[2],
		 0.0f, 0.0f, 0.0f, 1.0f,
	});

	matrix_multiply(dest, &M, &T);
}

#endif
