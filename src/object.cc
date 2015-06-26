#include "object.h"
#include "opengl.h"

Object::Object()
{
	mesh = 0;
	tex = 0;

	mtl.diffuse = Vector3(1, 1, 1);
	mtl.specular = Vector3(0, 0, 0);
	mtl.shininess = 60.0;
	mtl.alpha = 1.0;
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

Matrix4x4 &Object::tex_xform()
{
	return tex_matrix;
}

const Matrix4x4 &Object::tex_xform() const
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

void Object::draw() const
{
	if(!mesh) return;

	if(tex) {
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadTransposeMatrixf(tex_matrix[0]);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultTransposeMatrixf(matrix[0]);

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

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

bool Object::intersect(const Ray &ray, HitPoint *hit) const
{
	return false;	// TODO
}
