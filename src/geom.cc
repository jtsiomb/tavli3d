#include <algorithm>
#include <float.h>
#include "geom.h"

HitPoint::HitPoint()
{
	dist = 0;
	obj = 0;
	node = 0;
	data = 0;
}

GeomObject::~GeomObject()
{
}


Sphere::Sphere()
{
	radius = 1.0;
}

Sphere::Sphere(const Vec3 &cent, float radius)
	: center(cent)
{
	this->radius = radius;
}

GeomObjectType Sphere::get_type() const
{
	return GOBJ_SPHERE;
}

void Sphere::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	if(obj1->get_type() != GOBJ_SPHERE || obj2->get_type() != GOBJ_SPHERE) {
		fprintf(stderr, "Sphere::set_union: arguments must be spheres");
		return;
	}
	const Sphere *sph1 = (const Sphere*)obj1;
	const Sphere *sph2 = (const Sphere*)obj2;

	float dist = length(sph1->center - sph2->center);
	float surf_dist = dist - (sph1->radius + sph2->radius);
	float d1 = sph1->radius + surf_dist / 2.0;
	float d2 = sph2->radius + surf_dist / 2.0;
	float t = d1 / (d1 + d2);

	if(t < 0.0) t = 0.0;
	if(t > 1.0) t = 1.0;

	center = sph1->center * t + sph2->center * (1.0 - t);
	radius = std::max(dist * t + sph2->radius, dist * (1.0f - t) + sph1->radius);
}

void Sphere::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Sphere::intersection undefined\n");
}

bool Sphere::intersect(const Ray &ray, HitPoint *hit) const
{
	float a = dot(ray.dir, ray.dir);
	float b = 2.0 * ray.dir.x * (ray.origin.x - center.x) +
		2.0 * ray.dir.y * (ray.origin.y - center.y) +
		2.0 * ray.dir.z * (ray.origin.z - center.z);
	float c = dot(ray.origin, ray.origin) + dot(center, center) -
		2.0 * dot(ray.origin, center) - radius * radius;

	float discr = b * b - 4.0 * a * c;
	if(discr < 1e-4) {
		return false;
	}

	float sqrt_discr = sqrt(discr);
	float t0 = (-b + sqrt_discr) / (2.0 * a);
	float t1 = (-b - sqrt_discr) / (2.0 * a);

	if(t0 < 1e-4)
		t0 = t1;
	if(t1 < 1e-4)
		t1 = t0;

	float t = t0 < t1 ? t0 : t1;
	if(t < 1e-4) {
		return false;
	}

	// fill the HitPoint structure
	if(hit) {
		hit->obj = this;
		hit->dist = t;
		hit->pos = ray.origin + ray.dir * t;
		hit->normal = (hit->pos - center) / radius;
	}
	return true;
}

bool Sphere::contains(const Vec3 &pt) const
{
	return length_sq(pt - center) <= radius * radius;
}

float Sphere::distance(const Vec3 &v) const
{
	return length(v - center) - radius;
}

AABox::AABox()
{
}

AABox::AABox(const Vec3 &vmin, const Vec3 &vmax)
	: min(vmin), max(vmax)
{
}

GeomObjectType AABox::get_type() const
{
	return GOBJ_AABOX;
}

void AABox::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	if(obj1->get_type() != GOBJ_AABOX || obj2->get_type() != GOBJ_AABOX) {
		fprintf(stderr, "AABox::set_union: arguments must be AABoxes too\n");
		return;
	}
	const AABox *box1 = (const AABox*)obj1;
	const AABox *box2 = (const AABox*)obj2;

	min.x = std::min(box1->min.x, box2->min.x);
	min.y = std::min(box1->min.y, box2->min.y);
	min.z = std::min(box1->min.z, box2->min.z);

	max.x = std::max(box1->max.x, box2->max.x);
	max.y = std::max(box1->max.y, box2->max.y);
	max.z = std::max(box1->max.z, box2->max.z);
}

void AABox::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	if(obj1->get_type() != GOBJ_AABOX || obj2->get_type() != GOBJ_AABOX) {
		fprintf(stderr, "AABox::set_intersection: arguments must be AABoxes too\n");
		return;
	}
	const AABox *box1 = (const AABox*)obj1;
	const AABox *box2 = (const AABox*)obj2;

	for(int i=0; i<3; i++) {
		min[i] = std::max(box1->min[i], box2->min[i]);
		max[i] = std::min(box1->max[i], box2->max[i]);

		if(max[i] < min[i]) {
			max[i] = min[i];
		}
	}
}

bool AABox::intersect(const Ray &ray, HitPoint *hit) const
{
	Vec3 param[2] = {min, max};
	Vec3 inv_dir(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
	int sign[3] = {inv_dir.x < 0, inv_dir.y < 0, inv_dir.z < 0};

	float tmin = (param[sign[0]].x - ray.origin.x) * inv_dir.x;
	float tmax = (param[1 - sign[0]].x - ray.origin.x) * inv_dir.x;
	float tymin = (param[sign[1]].y - ray.origin.y) * inv_dir.y;
	float tymax = (param[1 - sign[1]].y - ray.origin.y) * inv_dir.y;

	if(tmin > tymax || tymin > tmax) {
		return false;
	}
	if(tymin > tmin) {
		tmin = tymin;
	}
	if(tymax < tmax) {
		tmax = tymax;
	}

	float tzmin = (param[sign[2]].z - ray.origin.z) * inv_dir.z;
	float tzmax = (param[1 - sign[2]].z - ray.origin.z) * inv_dir.z;

	if(tmin > tzmax || tzmin > tmax) {
		return false;
	}
	if(tzmin > tmin) {
		tmin = tzmin;
	}
	if(tzmax < tmax) {
		tmax = tzmax;
	}

	float t = tmin < 1e-4 ? tmax : tmin;
	if(t >= 1e-4) {

		if(hit) {
			hit->obj = this;
			hit->dist = t;
			hit->pos = ray.origin + ray.dir * t;

			float min_dist = FLT_MAX;
			Vec3 offs = min + (max - min) / 2.0;
			Vec3 local_hit = hit->pos - offs;

			static const Vec3 axis[] = {
				Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)
			};
			//int tcidx[][2] = {{2, 1}, {0, 2}, {0, 1}};

			for(int i=0; i<3; i++) {
				float dist = fabs((max[i] - offs[i]) - fabs(local_hit[i]));
				if(dist < min_dist) {
					min_dist = dist;
					hit->normal = axis[i] * (local_hit[i] < 0.0 ? 1.0 : -1.0);
					//hit->texcoord = Vec2(hit->pos[tcidx[i][0]], hit->pos[tcidx[i][1]]);
				}
			}
		}
		return true;
	}
	return false;
}

bool AABox::contains(const Vec3 &v) const
{
	return v.x >= min.x && v.y >= min.y && v.z >= min.z &&
		v.x <= max.x && v.y <= max.y && v.z <= max.z;
}

float AABox::distance(const Vec3 &v) const
{
	return 0.0;	// TODO
}


Plane::Plane()
	: normal(0.0, 1.0, 0.0)
{
}

Plane::Plane(const Vec3 &p, const Vec3 &norm)
	: pt(p)
{
	normal = normalize(norm);
}

Plane::Plane(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3)
	: pt(p1)
{
	normal = normalize(cross(p2 - p1, p3 - p1));
}

Plane::Plane(const Vec3 &normal, float dist)
{
	this->normal = normalize(normal);
	pt = this->normal * dist;
}

GeomObjectType Plane::get_type() const
{
	return GOBJ_PLANE;
}

void Plane::set_union(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Plane::set_union undefined\n");
}

void Plane::set_intersection(const GeomObject *obj1, const GeomObject *obj2)
{
	fprintf(stderr, "Plane::set_intersection undefined\n");
}

bool Plane::intersect(const Ray &ray, HitPoint *hit) const
{
	float ndotdir = dot(normal, ray.dir);
	if(fabs(ndotdir) < 1e-4) {
		return false;
	}

	if(hit) {
		Vec3 ptdir = pt - ray.origin;
		float t = dot(normal, ptdir) / ndotdir;

		hit->dist = t;
		hit->pos = ray.origin + ray.dir * t;
		hit->normal = normal;
		hit->obj = this;
	}
	return true;
}

bool Plane::contains(const Vec3 &v) const
{
	return dot(v, normal) <= 0.0;
}

float Plane::distance(const Vec3 &v) const
{
	return dot(v - pt, normal);
}
