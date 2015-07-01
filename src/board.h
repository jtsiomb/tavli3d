#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include "object.h"
#include "mesh.h"
#include "image.h"

#define NUM_SLOTS		20
#define PLAYER_PIECES	15
#define MAX_PIECES		(PLAYER_PIECES * 2)

enum { MINE, OTHER };

class Piece {
private:
	int prev_slot, prev_level;
	unsigned long move_start;

public:
	int owner, slot, level;

	Piece();

	void move_to(int slot, int level, bool anim = true);
};

// for slot bounds
class Quad {
public:
	Triangle tri0, tri1;

	Quad();
	Quad(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);

	bool intersect(const Ray &ray, HitPoint *hit = 0) const;
};

class Board {
private:
	Piece pieces[MAX_PIECES];
	int hist[NUM_SLOTS + 1];
	Quad slotbb[NUM_SLOTS];

	std::vector<Object*> obj;
	Object *piece_obj;

	Image img_wood, img_field, img_hinge;

	bool generate();
	bool generate_textures();

public:
	Board();
	~Board();

	bool init();
	void destroy();

	void clear();
	void setup();

	int slot_pieces(int slot) const;
	bool move_piece(int id, int slot, bool anim = true);

	Vector3 piece_pos(int slot, int level = 0) const;

	int slot_hit(const Ray &ray) const;

	void draw() const;
};

#endif	// BOARD_H_
