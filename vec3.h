#ifndef VEC3_H
#define VEC3_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define vec3_(x, y, z) \
(float[]){x,y,z}       \

#define vec3_zero() \
(float[]){0.0f, 0.0f, 0.0f} \

typedef float * vec3;

void vec3_copy(vec3 dest, vec3 src){
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

void vec3_cross(vec3 dest, vec3 a, vec3 b){
	vec3 temp = vec3_(0.0f, 0.0f, 0.0f);
	temp[0] = (a[1] * b[2]) - (a[2] * b[1]);
	temp[1] = (a[2] * b[0]) - (a[0] * b[2]);
	temp[2] = (a[0] * b[1]) - (a[1] * b[0]);
	vec3_copy(dest, temp);
}

void vec3_normalize(vec3 dest, vec3 src){
	float norm = 
	sqrt((src[0]*src[0])+(src[1]*src[1])+(src[2]*src[2]));
	dest[0] = src[0] / norm;
	dest[1] = src[1] / norm;
	dest[2] = src[2] / norm;
}

void vec3_subtract(vec3 dest, vec3 a, vec3 b){
	dest[0] = a[0] - b[0];
	dest[1] = a[1] - b[1];
	dest[2] = a[2] - b[2];
}

void vec3_print(vec3 src){
	printf("vec { %f, %f, %f } \n", src[0], src[1], src[2]);
}

#endif
