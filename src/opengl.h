#ifndef OPENGL_H_
#define OPENGL_H_

#include <GL/glew.h>

struct GLCaps {
	int shaders;
	int fsaa;
	int sep_spec;
	int fbo;
	int shadow;
};
extern struct GLCaps glcaps;

#ifdef __cplusplus
extern "C" {
#endif

int init_opengl();

#ifdef __cplusplus
}
#endif

#endif	/* OPENGL_H_ */
