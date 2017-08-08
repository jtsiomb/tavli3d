#ifndef OBJECT_H_
#define OBJECT_H_

#include "mesh.h"
#include "geom.h"
#include <gmath/gmath.h>

struct Material {
	Vec3 diffuse;
	Vec3 specular;
	float shininess;
	float alpha;

	Material();
};

struct RenderOps {
	bool zwrite;
	bool cast_shadows;
	bool transparent;

	RenderOps();
	void setup() const;
};

class Object {
private:
	Mesh *mesh;
	Mat4 matrix;
	unsigned int tex;
	Mat4 tex_matrix;
	unsigned int sdr;

public:
	Material mtl;
	RenderOps rop;

	Object();
	~Object();

	Mat4 &xform();
	const Mat4 &xform() const;

	Mat4 &tex_xform();
	const Mat4 &tex_xform() const;

	void set_mesh(Mesh *m);
	Mesh *get_mesh() const;

	void set_texture(unsigned int tex);
	void set_shader(unsigned int sdr);
	unsigned int get_shader() const;

	void draw() const;
	void draw_wire(const Vec4 &col = Vec4(1, 1, 1, 1)) const;
	void draw_vertices(const Vec4 &col = Vec4(1, 0.3, 0.2, 1)) const;
	void draw_normals(float len = 1.0, const Vec4 &col = Vec4(0.1, 0.2, 1.0, 1)) const;
	void draw_tangents(float len = 1.0, const Vec4 &col = Vec4(0.1, 1.0, 0.2, 1)) const;

	bool intersect(const Ray &ray, HitPoint *hit) const;
};

#endif	// OBJECT_H_
