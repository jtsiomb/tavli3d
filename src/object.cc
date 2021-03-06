#include "object.h"
#include "opengl.h"
#include "shadow.h"

Material::Material()
	: diffuse(1, 1, 1), specular(0, 0, 0)
{
	shininess = 60.0;
	alpha = 1.0;
}

RenderOps::RenderOps()
{
	zwrite = true;
	cast_shadows = true;
	transparent = false;
}

void RenderOps::setup() const
{
	if(!zwrite) {
		glDepthMask(0);
	}
	if(transparent) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}
}

Object::Object()
{
	mesh = 0;
	tex = 0;
	sdr = 0;
}

Object::~Object()
{
	delete mesh;
}

Mat4 &Object::xform()
{
	return matrix;
}

const Mat4 &Object::xform() const
{
	return matrix;
}

Mat4 &Object::tex_xform()
{
	return tex_matrix;
}

const Mat4 &Object::tex_xform() const
{
	return tex_matrix;
}

void Object::set_mesh(Mesh *m)
{
	this->mesh = m;
}

Mesh *Object::get_mesh() const
{
	return mesh;
}

void Object::set_texture(unsigned int tex)
{
	this->tex = tex;
}

void Object::set_shader(unsigned int sdr)
{
	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader) {
		this->sdr = sdr;
	}
}

unsigned int Object::get_shader() const
{
	return sdr;
}

void Object::draw() const
{
	if(!mesh) return;

	if(shadow_pass && !rop.cast_shadows) {
		return;
	}

	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
	rop.setup();

	if(glcaps.shaders) {
		if(sdr) {
			if(!shadow_pass) {
				glUseProgram(sdr);
			}
		} else {
			glUseProgram(0);
		}
	}

	if(tex) {
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadMatrixf(tex_matrix[0]);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix[0]);

	float dcol[] = {mtl.diffuse.x, mtl.diffuse.y, mtl.diffuse.z, mtl.alpha};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dcol);
	float scol[] = {mtl.specular.x, mtl.specular.y, mtl.specular.z, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, scol);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mtl.shininess);

	mesh->draw();

	if(tex) {
		glDisable(GL_TEXTURE_2D);

		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
	}

	if(sdr) {
		glUseProgram(0);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void Object::draw_wire(const Vec4 &col) const
{
	if(shadow_pass) return;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glUseProgram(0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix[0]);

	glColor4f(col.x, col.y, col.z, col.w);
	mesh->draw_wire();

	glPopMatrix();
	glPopAttrib();
}

void Object::draw_vertices(const Vec4 &col) const
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glUseProgram(0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix[0]);

	glColor4f(col.x, col.y, col.z, col.w);
	mesh->draw_vertices();

	glPopMatrix();
	glPopAttrib();
}

void Object::draw_normals(float len, const Vec4 &col) const
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix[0]);

	glColor4f(col.x, col.y, col.z, col.w);
	mesh->set_vis_vecsize(len);
	mesh->draw_normals();

	glPopMatrix();
	glPopAttrib();
}

void Object::draw_tangents(float len, const Vec4 &col) const
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix[0]);

	glColor4f(col.x, col.y, col.z, col.w);
	mesh->set_vis_vecsize(len);
	mesh->draw_tangents();

	glPopMatrix();
	glPopAttrib();
}

bool Object::intersect(const Ray &ray, HitPoint *hit) const
{
	return false;	// TODO
}
