#ifndef OBJECT_H_
#define OBJECT_H_

#include "mesh.h"

class Object {
private:
	Mesh *mesh;
	Matrix4x4 matrix;

public:
	Object();
	~Object();

	Matrix4x4 &xform();
	const Matrix4x4 &xform() const;

	void set_mesh(Mesh *m);
	Mesh *get_mesh() const;

	void draw() const;
};

#endif	// OBJECT_H_
