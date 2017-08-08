#ifndef REVOL_H_
#define REVOL_H_

#include <gmath/gmath.h>

struct BezCurve {
	int numcp;
	Vec2 *cp;
	float scale;
};

Vec2 bezier_revol(float u, float v, void *cls);
Vec2 bezier_revol_normal(float u, float v, void *cls);

#endif	// REVOL_H_
