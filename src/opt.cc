#include <stdio.h>
#include "opt.h"

Options opt;

bool init_options(int argc, char **argv)
{
	// TODO read config files, parse args, etc...

	opt.xres = 1280;
	opt.yres = 800;
	opt.fullscreen = false;
	opt.shadows = true;
	opt.reflections = true;

	opt.def_username = opt.saved_passwd = 0;

	opt.piece_color[0] = Vec3(0.3, 0.35, 0.6);
	opt.piece_color[1] = Vec3(1.0, 0.75, 0.5);

	return true;
}
