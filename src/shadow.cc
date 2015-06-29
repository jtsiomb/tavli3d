#include <assert.h>
#include "opengl.h"
#include "shadow.h"
#include "vmath/vmath.h"

bool shadow_pass;

static int tex_sz, prev_vp[4];
static unsigned int fbo, depth_tex, rb_color;
static Matrix4x4 shadow_mat;

bool init_shadow(int sz)
{
	if(!glcaps.fbo || !glcaps.shadow) {
		return false;
	}

	tex_sz = sz;
	printf("initializing shadow buffer (%dx%d)\n", tex_sz, tex_sz);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	shadow_pass = true;

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColorMask(0, 0, 0, 0);
	glDepthMask(1);

	Matrix4x4 viewmat;
	glGetFloatv(GL_MODELVIEW_MATRIX, viewmat[0]);
	viewmat.transpose();

	Matrix4x4 lt_viewmat, lt_projmat;
	lt_projmat.set_perspective(DEG_TO_RAD(lfov) * 2.0, 1.0, 8.0, 50.0);
	lt_viewmat.set_lookat(lpos, ltarg, Vector3(0, 1, 0));
	shadow_mat = lt_projmat * lt_viewmat * viewmat.inverse();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadTransposeMatrixf(lt_projmat[0]);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadTransposeMatrixf(lt_viewmat[0]);

	glGetIntegerv(GL_VIEWPORT, prev_vp);
	glViewport(0, 0, tex_sz, tex_sz);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glPolygonOffset(2, 2);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(0);
}


void end_shadow_pass()
{
	shadow_pass = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(lfov * 2.0, 1.0, 0.5, 50.0);
	gluLookAt(lpos.x, lpos.y, lpos.z, ltarg.x, ltarg.y, ltarg.z, 0, 1, 0);

	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);

	Matrix4x4 res;
	memcpy(res[0], mat, sizeof mat);
	res.transpose();
	return res;
	*/
}

unsigned int get_shadow_tex()
{
	return depth_tex;
}
