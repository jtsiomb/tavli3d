#ifndef GEOMOBJ_H_
#define GEOMOBJ_H_

#include <gmath/gmath.h>

enum GeomObjectType {
	GOBJ_UNKNOWN,
	GOBJ_SPHERE,
	GOBJ_AABOX,
	GOBJ_PLANE
};

class GeomObject;
class SceneNode;

struct HitPoint {
	float dist;			// parametric distance along the ray
	Vec3 pos;			// position of intersection (orig + dir * dist)
	Vec3 normal;		// normal at the point of intersection
	Ray ray, local_ray;
	const GeomObject *obj;	// pointer to the intersected geom-object
	const SceneNode *node;
	void *data;			// place to hang extra data

	HitPoint();
};

class GeomObject {
public:
	virtual ~GeomObject();
	virtual GeomObjectType get_type() const = 0;

	virtual void set_union(const GeomObject *obj1, const GeomObject *obj2) = 0;
	virtual void set_intersection(const GeomObject *obj1, const GeomObject *obj2) = 0;

	virtual bool intersect(const Ray &ray, HitPoint *hit = 0) const = 0;
	virtual bool contains(const Vec3 &pt) const = 0;

	virtual float distance(const Vec3 &v) const = 0;
};

class Sphere : public GeomObject {
public:
	Vec3 center;
	float radius;

	Sphere();
	Sphere(const Vec3 &center, float radius);

	GeomObjectType get_type() const;

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
	bool contains(const Vec3 &pt) const;

	float distance(const Vec3 &v) const;
};

class AABox : public GeomObject {
public:
	Vec3 min, max;

	AABox();
	AABox(const Vec3 &min, const Vec3 &max);

	GeomObjectType get_type() const;

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
	bool contains(const Vec3 &pt) const;

	float distance(const Vec3 &v) const;
};

class Plane : public GeomObject {
public:
	Vec3 pt, normal;

	Plane();
	Plane(const Vec3 &pt, const Vec3 &normal);
	Plane(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3);
	Plane(const Vec3 &normal, float dist);

	GeomObjectType get_type() const;

	void set_union(const GeomObject *obj1, const GeomObject *obj2);
	void set_intersection(const GeomObject *obj1, const GeomObject *obj2);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
	bool contains(const Vec3 &pt) const;

	float distance(const Vec3 &v) const;
};

#endif	// GEOMOBJ_H_
