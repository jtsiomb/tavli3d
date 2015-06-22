#include "object.h"
#include "opengl.h"

Object::Object()
{
	mesh = 0;
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

void Object::draw() const
{
	if(!mesh) return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultTransposeMatrixf(matrix[0]);

	mesh->draw();

	glPopMatrix();
}
