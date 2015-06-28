#ifndef REVOL_H_
#define REVOL_H_

#include "vmath/vmath.h"

struct BezCurve {
	int numcp;
	vec2_t *cp;
	float scale;
};

Vector2 bezier_revol(float u, float v, void *cls);
Vector2 bezier_revol_normal(float u, float v, void *cls);

#endif	// REVOL_H_
