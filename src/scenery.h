#ifndef SCENERY_H_
#define SCENERY_H_

#include "scene.h"

bool init_scenery();
void destroy_scenery();

void draw_scenery(unsigned int flags = DRAW_ALL);

#endif	// SCENERY_H_
