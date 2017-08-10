#include "opengl.h"

struct GLCaps glcaps;

int init_opengl()
{
	/*glewInit();

	glcaps.shaders = GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader;
	glcaps.fsaa = GLEW_ARB_multisample;
	glcaps.sep_spec = GLEW_EXT_separate_specular_color;
	glcaps.fbo = GLEW_ARB_framebuffer_object;
	glcaps.shadow = GLEW_ARB_shadow | GLEW_SGIX_shadow;
	glcaps.gen_mipmaps = GLEW_SGIS_generate_mipmap;
	glcaps.aniso = GLEW_EXT_texture_filter_anisotropic;
	*/
	glcaps.shaders = 1;
	glcaps.fsaa = 1;
	glcaps.sep_spec = 1;
	glcaps.fbo = 1;
	glcaps.shadow = 1;
	glcaps.gen_mipmaps = 1;
	glcaps.aniso = 1;

	glcaps.max_aniso = -1;
	if(glcaps.aniso) {
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glcaps.max_aniso);
	}

	return 0;
}
