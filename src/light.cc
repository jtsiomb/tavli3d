#include "light.h"
#include "opengl.h"

Light::Light()
	: color(1, 1, 1)
{
}

void Light::setup(int idx) const
{
	float lpos[] = {pos.x, pos.y, pos.z, 1.0};
	float col[] = {color.x, color.y, color.z, 1.0};

	glLightfv(GL_LIGHT0 + idx, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0 + idx, GL_DIFFUSE, col);
	glLightfv(GL_LIGHT0 + idx, GL_SPECULAR, col);
}
