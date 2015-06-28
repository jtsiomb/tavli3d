#ifndef OPT_H_
#define OPT_H_

#include "vmath/vmath.h"

struct Options {
	int xres, yres;
	bool fullscreen;

	char *def_username, *saved_passwd;

	vec3_t piece_color[2];
};

extern Options opt;

bool init_options(int argc, char **argv);

#endif	/* OPT_H_ */
