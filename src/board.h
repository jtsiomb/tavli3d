#ifndef BOARD_H_
#define BOARD_H_

#include "mesh.h"

#define NUM_SLOTS	24
#define MAX_PUCKS	30

enum { EMPTY = 0, MINE, OTHER };

class Board {
private:
	int slots[NUM_SLOTS][MAX_PUCKS];
	Mesh *board_mesh, *puck_mesh;

public:
	Board();
	~Board();

	bool init();
	void destroy();

	void clear();

	void draw() const;
};

#endif	// BOARD_H_
