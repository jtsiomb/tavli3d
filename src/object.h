#ifndef OBJECT_H_
#define OBJECT_H_

#include "mesh.h"
#include "geom.h"
#include "vmath/vmath.h"

struct Material {
	Vector3 diffuse;
	Vector3 specular;
	float shininess;
	float alpha;
};

class Object {
private:
	Mesh *mesh;
	Matrix4x4 matrix;
	unsigned int tex;
	Matrix4x4 tex_matrix;

public:
	Material mtl;

	Object();
	~Object();

	Matrix4x4 &xform();
	const Matrix4x4 &xform() const;

	Matrix4x4 &tex_xform();
	const Matrix4x4 &tex_xform() const;

	void set_mesh(Mesh *m);
	Mesh *get_mesh() const;

	void set_texture(unsigned int tex);

	void draw() const;

	bool intersect(const Ray &ray, HitPoint *hit) const;
};

#endif	// OBJECT_H_
