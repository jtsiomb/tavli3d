#ifndef SNODE_H_
#define SNODE_H_

#include <vector>
#include "object.h"
#include "vmath/vmath.h"
#include "geom.h"

class SceneNode {
private:
	Vector3 pos;
	Quaternion rot;
	Vector3 scale;

	std::vector<Object*> obj;

	SceneNode *parent;
	std::vector<SceneNode*> children;

	Matrix4x4 xform;
	Matrix4x4 inv_xform;

public:
	SceneNode();
	explicit SceneNode(Object *obj);

	void add_child(SceneNode *node);
	bool remove_child(SceneNode *node);

	int get_num_children() const;
	SceneNode *get_child(int idx) const;

	SceneNode *get_parent() const;

	void add_object(Object *obj);
	int get_num_objects() const;
	Object *get_object(int idx) const;

	void set_position(const Vector3 &pos);
	void set_rotation(const Quaternion &rot);
	void set_scaling(const Vector3 &scale);

	const Vector3 &get_node_position() const;
	const Quaternion &get_node_rotation() const;
	const Vector3 &get_node_scaling() const;

	Vector3 get_position() const;
	Quaternion get_rotation() const;
	Vector3 get_scaling() const;

	const Matrix4x4 &get_matrix() const;
	const Matrix4x4 &get_inv_matrix() const;

	void update_node(long msec = 0);
	void update(long msec = 0);

	bool intersect(const Ray &ray, HitPoint *hit) const;
};

#endif	// SNODE_H_
