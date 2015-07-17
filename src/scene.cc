#include "scene.h"
#include "opengl.h"
#include "opt.h"
#include "game.h"

static int max_lights = -1;

Scene::~Scene()
{
	clear();
}

void Scene::clear()
{
	for(size_t i=0; i<objects.size(); i++) {
		delete objects[i];
	}
	objects.clear();

	for(size_t i=0; i<lights.size(); i++) {
		delete lights[i];
	}
	lights.clear();
}

void Scene::add_object(Object *obj)
{
	objects.push_back(obj);
}

void Scene::add_lights(Light *lt)
{
	lights.push_back(lt);
}

void Scene::draw(unsigned int flags) const
{
	if(max_lights == -1) {
		glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
		printf("max lights: %d\n", max_lights);
	}

	for(size_t i=0; i<lights.size(); i++) {
		lights[i]->setup(i);
	}

	unsigned int sdr = opt.shadows && sdr_shadow ? sdr_shadow : sdr_phong;

	for(size_t i=0; i<objects.size(); i++) {
		unsigned int mask = objects[i]->rop.transparent ? DRAW_TRANSPARENT : DRAW_SOLID;
		if(mask & flags) {
			if(objects[i]->get_shader()) {
				objects[i]->set_shader(sdr);
			}

			if(wireframe) {
				objects[i]->draw_wire();
			} else {
				objects[i]->draw();
			}
		}
	}
}
