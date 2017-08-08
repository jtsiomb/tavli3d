#ifndef SNODE_H_
#define SNODE_H_

#include <vector>
#include "object.h"
#include <gmath/gmath.h>
#include "geom.h"

class SceneNode {
private:
	Vec3 pos;
	Quat rot;
	Vec3 scale;

	std::vector<Object*> obj;

	SceneNode *parent;
	std::vector<SceneNode*> children;

	Mat4 xform;
	Mat4 inv_xform;

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

	void set_position(const Vec3 &pos);
	void set_rotation(const Quat &rot);
	void set_scaling(const Vec3 &scale);

	const Vec3 &get_node_position() const;
	const Quat &get_node_rotation() const;
	const Vec3 &get_node_scaling() const;

	Vec3 get_position() const;
	Quat get_rotation() const;
	Vec3 get_scaling() const;

	const Mat4 &get_matrix() const;
	const Mat4 &get_inv_matrix() const;

	void update_node(long msec = 0);
	void update(long msec = 0);

	bool intersect(const Ray &ray, HitPoint *hit) const;
};

#endif	// SNODE_H_
