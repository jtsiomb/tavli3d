#include <float.h>
#include <assert.h>
#include <algorithm>
#include "snode.h"

SceneNode::SceneNode()
	: scale(1, 1, 1)
{
	parent = 0;
}

SceneNode::SceneNode(Object *obj)
	: scale(1, 1, 1)
{
	parent = 0;
	add_object(obj);
}

void SceneNode::add_child(SceneNode *node)
{
	if(node->parent) {
		if(node->parent == this) {
			return;
		}
		node->parent->remove_child(node);
	}

	children.push_back(node);
	node->parent = this;
}

bool SceneNode::remove_child(SceneNode *node)
{
	for(size_t i=0; i<children.size(); i++) {
		if(children[i] == node) {
			assert(node->parent == this);
			node->parent = 0;
			return true;
		}
	}
	return false;
}

int SceneNode::get_num_children() const
{
	return (int)children.size();
}

SceneNode *SceneNode::get_child(int idx) const
{
	return children[idx];
}

SceneNode *SceneNode::get_parent() const
{
	return parent;
}

void SceneNode::add_object(Object *obj)
{
	if(std::find(this->obj.begin(), this->obj.end(), obj) == this->obj.end()) {
		this->obj.push_back(obj);
	}
}

int SceneNode::get_num_objects() const
{
	return (int)obj.size();
}

Object *SceneNode::get_object(int idx) const
{
	return obj[idx];
}

void SceneNode::set_position(const Vector3 &pos)
{
	this->pos = pos;
}

void SceneNode::set_rotation(const Quaternion &rot)
{
	this->rot = rot;
}

void SceneNode::set_scaling(const Vector3 &scale)
{
	this->scale = scale;
}


const Vector3 &SceneNode::get_node_position() const
{
	return pos;
}

const Quaternion &SceneNode::get_node_rotation() const
{
	return rot;
}

const Vector3 &SceneNode::get_node_scaling() const
{
	return scale;
}


Vector3 SceneNode::get_position() const
{
	return Vector3(0, 0, 0).transformed(xform);
}

Quaternion SceneNode::get_rotation() const
{
	return rot;	// TODO
}

Vector3 SceneNode::get_scaling() const
{
	return scale;	// TODO
}

const Matrix4x4 &SceneNode::get_matrix() const
{
	return xform;
}

const Matrix4x4 &SceneNode::get_inv_matrix() const
{
	return inv_xform;
}


void SceneNode::update_node(long msec)
{
	xform.reset_identity();
	xform.translate(pos);
	xform.rotate(rot);
	xform.scale(scale);

	if(parent) {
		xform = parent->xform * xform;
	}
	inv_xform = xform.inverse();
}

void SceneNode::update(long msec)
{
	update_node(msec);

	for(size_t i=0; i<children.size(); i++) {
		children[i]->update(msec);
	}
}


bool SceneNode::intersect(const Ray &ray, HitPoint *hit) const
{
	Ray local_ray = ray.transformed(inv_xform);

	HitPoint nearest;
	nearest.dist = FLT_MAX;
	for(size_t i=0; i<obj.size(); i++) {
		if(obj[i]->intersect(local_ray, hit)) {
			if(!hit) return true;
			if(hit->dist < nearest.dist) {
				nearest = *hit;
				nearest.node = this;
			}
		}
	}

	for(size_t i=0; i<children.size(); i++) {
		if(children[i]->intersect(ray, hit)) {
			if(!hit) return true;
			if(hit->dist < nearest.dist) {
				nearest = *hit;
			}
		}
	}

	if(nearest.dist < FLT_MAX) {
		*hit = nearest;
		hit->ray = ray;
		return true;
	}
	return false;
}
