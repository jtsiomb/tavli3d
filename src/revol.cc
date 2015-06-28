#include <algorithm>
#include "revol.h"

Vector2 bezier_revol(float u, float v, void *cls)
{
	BezCurve *curve = (BezCurve*)cls;
	int nseg = (curve->numcp - 1) / 2;

	if(v >= 1.0) v = 1.0 - 1e-6;
	int cidx = std::min((int)(v * nseg), nseg - 1);
	float t = fmod(v * (float)nseg, 1.0);

	const vec2_t *cp = curve->cp + cidx * 2;

	float resx = bezier(cp[0].x, cp[1].x, cp[1].x, cp[2].x, t);
	float resy = bezier(cp[0].y, cp[1].y, cp[1].y, cp[2].y, t);
	return Vector2(resx * curve->scale, resy * curve->scale);
}

Vector2 bezier_revol_normal(float u, float v, void *cls)
{
	BezCurve *curve = (BezCurve*)cls;
	int nseg = (curve->numcp - 1) / 2;

	if(v >= 1.0) v = 1.0 - 1e-6;
	int cidx = std::min((int)(v * nseg), nseg - 1);
	float t = fmod(v * (float)nseg, 1.0);

	const vec2_t *cp = curve->cp + cidx * 2;
	Vector2 cp0 = cp[0];
	Vector2 cp1 = cp[1];
	Vector2 cp2 = cp[2];

	Vector2 pprev, pnext;
	for(int i=0; i<2; i++) {
		pprev[i] = bezier(cp0[i], cp1[i], cp1[i], cp2[i], t - 0.05);
		pnext[i] = bezier(cp0[i], cp1[i], cp1[i], cp2[i], t + 0.05);
	}

	float tx = pnext.x - pprev.x;
	float ty = pnext.y - pprev.y;

	return Vector2(-ty, tx);
}
