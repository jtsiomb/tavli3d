#ifndef PNOISE_H_
#define PNOISE_H_

float dbg_noise2(float x, float y);
float pnoise2(float x, float y, int periodx, int periody);
float pturbulence2(float x, float y, int periodx, int periody, int octaves);

#endif	// PNOISE_H_
