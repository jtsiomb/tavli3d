#include <stdio.h>
#include "opt.h"

Options opt;

bool init_options(int argc, char **argv)
{
	// TODO read config files, parse args, etc...

	opt.xres = 1280;
	opt.yres = 800;
	opt.fullscreen = false;
	opt.def_username = opt.saved_passwd = 0;

	opt.piece_color[0] = v3_cons(0.3, 0.35, 0.6);
	opt.piece_color[1] = v3_cons(1.0, 0.75, 0.5);

	return true;
}
