#include <stdlib.h>
#include "pnoise.h"
#include "vmath/vmath.h"

#define B	0x100
#define BM	0xff
#define N	0x1000
#define NP	12
#define NM	0xfff

#define s_curve(t)	((t) * (t) * (3.0f - 2.0f * (t)))
#define setup(elem, b0, b1, r0, r1) \
	do {							\
		float t = elem + N;		\
		b0 = ((int)t) & BM;			\
		b1 = (b0 + 1) & BM;			\
		r0 = t - (int)t;			\
		r1 = r0 - 1.0f;				\
	} while(0)

#define setup_p(elem, b0, b1, r0, r1, p) \
	do {							\
		float t = elem + N;		\
		b0 = (((int)t) & BM) % p;	\
		b1 = ((b0 + 1) & BM) % p;	\
		r0 = t - (int)t;			\
		r1 = r0 - 1.0f;				\
	} while(0)

static int perm[B + B + 2];
static vec2_t grad2[B + B + 2];
static bool tables_valid;

static void init_noise()
{
	for(int i=0; i<B; i++) {
		perm[i] = i;

		grad2[i].x = (float)((rand() % (B + B)) - B) / B;
		grad2[i].y = (float)((rand() % (B + B)) - B) / B;
		grad2[i] = v2_normalize(grad2[i]);
	}

	for(int i=0; i<B; i++) {
		int rand_idx = rand() % B;

		int tmp = perm[i];
		perm[i] = perm[rand_idx];
		perm[rand_idx] = tmp;
	}

	for(int i=0; i<B+2; i++) {
		perm[B + i] = perm[i];
		grad2[B + i] = grad2[i];
	}
}

#define lerp(a, b, t)	((a) + ((b) - (a)) * t)

float dbg_noise2(float x, float y)
{
	if(!tables_valid) {
		init_noise();
		tables_valid = true;
	}

	int bx0, bx1, by0, by1;
	float rx0, rx1, ry0, ry1;
	setup(x, bx0, bx1, rx0, rx1);
	setup(y, by0, by1, ry0, ry1);

	int i = perm[bx0];
	int j = perm[bx1];

	int b00 = perm[i + by0];
	int b10 = perm[j + by0];
	int b01 = perm[i + by1];
	int b11 = perm[j + by1];

	float sx = s_curve(rx0);
	float sy = s_curve(ry0);

	vec2_t g00 = grad2[b00];
	vec2_t g10 = grad2[b10];
	vec2_t g01 = grad2[b01];
	vec2_t g11 = grad2[b11];

	float u = g00.x * rx0 + g00.y * ry0;
	float v = g10.x * rx1 + g10.y * ry0;
	float a = lerp(u, v, sx);

	u = g01.x * rx0 + g01.y * ry1;
	v = g11.x * rx1 + g11.y * ry1;
	float b = lerp(u, v, sx);

	return lerp(a, b, sy);
}

float pnoise2(float x, float y, int periodx, int periody)
{
	if(!tables_valid) {
		init_noise();
		tables_valid = true;
	}

	int bx0, bx1, by0, by1;
	float rx0, rx1, ry0, ry1;
	setup_p(x, bx0, bx1, rx0, rx1, periodx);
	setup_p(y, by0, by1, ry0, ry1, periody);

	int i = perm[bx0];
	int j = perm[bx1];

	int b00 = perm[i + by0];
	int b10 = perm[j + by0];
	int b01 = perm[i + by1];
	int b11 = perm[j + by1];

	float sx = s_curve(rx0);
	float sy = s_curve(ry0);

	vec2_t g00 = grad2[b00];
	vec2_t g10 = grad2[b10];
	vec2_t g01 = grad2[b01];
	vec2_t g11 = grad2[b11];

	float u = g00.x * rx0 + g00.y * ry0;
	float v = g10.x * rx1 + g10.y * ry0;
	float a = lerp(u, v, sx);

	u = g01.x * rx0 + g01.y * ry1;
	v = g11.x * rx1 + g11.y * ry1;
	float b = lerp(u, v, sx);

	return lerp(a, b, sy);
}

float pturbulence2(float x, float y, int periodx, int periody, int octaves)
{
	int i;
	float res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += fabs(pnoise2(x * freq, y * freq, periodx, periody) / freq);
		freq *= 2.0f;
		periodx *= 2;
		periody *= 2;
	}
	return res;
}
