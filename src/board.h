#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include "object.h"
#include "image.h"

#define NUM_SLOTS	24
#define MAX_PUCKS	30

enum { EMPTY = 0, MINE, OTHER };

class Board {
private:
	int slots[NUM_SLOTS][MAX_PUCKS];
	std::vector<Object*> obj;
	Object *puck_obj;

	Image img_wood, img_field, img_hinge;

	bool generate();
	bool generate_textures();

public:
	Board();
	~Board();

	bool init();
	void destroy();

	void clear();

	void draw() const;
};

#endif	// BOARD_H_
