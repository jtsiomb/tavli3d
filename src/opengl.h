#ifndef OPENGL_H_
#define OPENGL_H_

#define GL_GLEXT_PROTOTYPES 1

/*#include <GL/glew.h> */
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

struct GLCaps {
	int shaders;
	int fsaa;
	int sep_spec;
	int fbo;
	int shadow;
	int gen_mipmaps;
	int aniso, max_aniso;
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
