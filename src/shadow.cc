#include <assert.h>
#include "opengl.h"
#include "shadow.h"
#include "vmath/vmath.h"

static int tex_sz, prev_vp[4];
static unsigned int fbo, depth_tex, rb_color;
static Matrix4x4 shadow_mat;

bool init_shadow(int sz)
{
	if(!glcaps.fbo) {
		return true;
	}

	tex_sz = sz;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, tex_sz, tex_sz, 0,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	assert(glGetError() == GL_NO_ERROR);

	glDrawBuffer(GL_FALSE);
	glReadBuffer(GL_FALSE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "incomplete framebuffer\n");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	assert(glGetError() == GL_NO_ERROR);

	return true;
}

void destroy_shadow()
{
	glDeleteTextures(1, &depth_tex);
	glDeleteRenderbuffers(1, &rb_color);
	glDeleteFramebuffers(1, &fbo);
}

void begin_shadow_pass(const Vector3 &lpos, const Vector3 &ltarg, float lfov)
{
	Matrix4x4 viewmat, projmat;

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColorMask(0, 0, 0, 0);
	glDepthMask(1);

	projmat.set_perspective(DEG_TO_RAD(lfov) * 2.0, 1.0, 0.5, 500.0);
	viewmat.set_lookat(lpos, ltarg, Vector3(0, 1, 0));
	shadow_mat = viewmat * projmat;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadTransposeMatrixf(projmat[0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadTransposeMatrixf(viewmat[0]);

	glGetIntegerv(GL_VIEWPORT, prev_vp);
	glViewport(0, 0, tex_sz, tex_sz);

	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}


void end_shadow_pass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);

	glViewport(prev_vp[0], prev_vp[1], prev_vp[2], prev_vp[3]);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

Matrix4x4 get_shadow_matrix()
{
	return shadow_mat;
}

unsigned int get_shadow_tex()
{
	return depth_tex;
}
