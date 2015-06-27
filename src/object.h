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
	void draw_wire(const Vector4 &col = Vector4(1, 1, 1, 1)) const;
	void draw_vertices(const Vector4 &col = Vector4(1, 0.3, 0.2, 1)) const;
	void draw_normals(float len = 1.0, const Vector4 &col = Vector4(0.1, 0.2, 1.0, 1)) const;
	void draw_tangents(float len = 1.0, const Vector4 &col = Vector4(0.1, 1.0, 0.2, 1)) const;

	bool intersect(const Ray &ray, HitPoint *hit) const;
};

#endif	// OBJECT_H_
