#ifndef OPT_H_
#define OPT_H_

#include <gmath/gmath.h>

struct Options {
	int xres, yres;
	bool fullscreen;
	bool shadows, reflections;

	char *def_username, *saved_passwd;

	Vec3 piece_color[2];
};

extern Options opt;

bool init_options(int argc, char **argv);

#endif	/* OPT_H_ */
