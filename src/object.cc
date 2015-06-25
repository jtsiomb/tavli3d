#include "object.h"
#include "opengl.h"

Object::Object()
{
	mesh = 0;
	tex = 0;
}

Object::~Object()
{
	delete mesh;
}

Matrix4x4 &Object::xform()
{
	return matrix;
}

const Matrix4x4 &Object::xform() const
{
	return matrix;
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

void Object::draw() const
{
	if(!mesh) return;

	if(tex) {
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultTransposeMatrixf(matrix[0]);

	mesh->draw();

	glPopMatrix();

	if(tex) {
		glDisable(GL_TEXTURE_2D);
	}
}

bool Object::intersect(const Ray &ray, HitPoint *hit) const
{
	return false;	// TODO
}
