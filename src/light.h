#ifndef LIGHT_H_
#define LIGHT_H_

#include "vmath/vmath.h"

class Light {
public:
	Vector3 pos;
	Vector3 color;

	Light();

	void setup(int idx = 0) const;
};

#endif	// LIGHT_H_
