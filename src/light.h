#ifndef LIGHT_H_
#define LIGHT_H_

#include <gmath/gmath.h>

class Light {
public:
	Vec3 pos;
	Vec3 color;

	Light();

	void setup(int idx = 0) const;
};

#endif	// LIGHT_H_
