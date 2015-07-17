#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "object.h"
#include "light.h"

enum {
	DRAW_SOLID = 1,
	DRAW_TRANSPARENT = 2,
	DRAW_ALL = 0x7fffffff
};

class Scene {
public:
	std::vector<Object*> objects;
	std::vector<Light*> lights;

	~Scene();

	void clear();

	void add_object(Object *obj);
	void add_lights(Light *lt);

	void draw(unsigned int flags = DRAW_ALL) const;
};

#endif	// SCENE_H_
