#include "opengl.h"

struct GLCaps glcaps;

int init_opengl()
{
	glewInit();

	glcaps.shaders = GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader;
	glcaps.fsaa = GLEW_ARB_multisample;
	glcaps.sep_spec = GLEW_EXT_separate_specular_color;

	return 0;
}
