#ifndef SHADOW_H_
#define SHADOW_H_

#include "vmath/vmath.h"

extern bool shadow_pass;

bool init_shadow(int sz);
void destroy_shadow();

void begin_shadow_pass(const Vector3 &lpos, const Vector3 &ltarg, float lfov);
void end_shadow_pass();

Matrix4x4 get_shadow_matrix();
unsigned int get_shadow_tex();

#endif	// SHADOW_H_
