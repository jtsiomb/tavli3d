#include "opengl.h"
#include "board.h"
#include "meshgen.h"


Board::Board()
{
	puck_obj = 0;
	clear();
}

Board::~Board()
{
	destroy();
}

bool Board::init()
{
	if(!generate()) {
		return false;
	}

	return true;
}

void Board::destroy()
{
	for(size_t i=0; i<obj.size(); i++) {
		delete obj[i];
	}
	obj.clear();

	delete puck_obj;
	puck_obj = 0;
}

void Board::clear()
{
	memset(slots, 0, sizeof slots);
}

void Board::draw() const
{
	for(size_t i=0; i<obj.size(); i++) {
		obj[i]->draw();
	}
}

#define HSIZE	1.0
#define VSIZE	(2.0 * HSIZE)
#define BOT_THICKNESS	(HSIZE * 0.01)
#define WALL_THICKNESS	(HSIZE * 0.05)
#define WALL_HEIGHT		(HSIZE * 0.1)
#define GAP				(HSIZE * 0.025)
#define HINGE_RAD		(GAP * 0.5)
#define HINGE_HEIGHT	(VSIZE * 0.075)

bool Board::generate()
{
	Matrix4x4 xform;

	obj.clear();

	// generate bottom
	Mesh *bottom = new Mesh;
	gen_box(bottom, HSIZE, BOT_THICKNESS, HSIZE * 2.0);
	xform.set_translation(Vector3(0, -BOT_THICKNESS / 2.0, 0));
	bottom->apply_xform(xform);

	Object *obottom = new Object;
	obottom->set_mesh(bottom);
	obj.push_back(obottom);


	// generate the 4 sides
	Mesh *sides = new Mesh;
	gen_box(sides, WALL_THICKNESS, WALL_HEIGHT, VSIZE + WALL_THICKNESS * 2);
	xform.set_translation(Vector3(-(HSIZE + WALL_THICKNESS) / 2.0,
				WALL_HEIGHT / 2.0 - BOT_THICKNESS, 0));
	sides->apply_xform(xform);

	Mesh tmp;
	gen_box(&tmp, WALL_THICKNESS, WALL_HEIGHT, VSIZE + WALL_THICKNESS * 2);
	xform.set_translation(Vector3((HSIZE + WALL_THICKNESS) / 2.0,
				WALL_HEIGHT / 2.0 - BOT_THICKNESS, 0));
	tmp.apply_xform(xform);
	sides->append(tmp);
	tmp.clear();

	gen_box(&tmp, HSIZE, WALL_HEIGHT, WALL_THICKNESS);
	xform.set_translation(Vector3(0, WALL_HEIGHT / 2.0 - BOT_THICKNESS,
				(VSIZE + WALL_THICKNESS) / 2.0));
	tmp.apply_xform(xform);
	sides->append(tmp);
	tmp.clear();

	gen_box(&tmp, HSIZE, WALL_HEIGHT, WALL_THICKNESS);
	xform.set_translation(Vector3(0, WALL_HEIGHT / 2.0 - BOT_THICKNESS,
				-(VSIZE + WALL_THICKNESS) / 2.0));
	tmp.apply_xform(xform);
	sides->append(tmp);
	tmp.clear();

	Object *osides = new Object;
	osides->set_mesh(sides);
	obj.push_back(osides);


	// generate the hinges
	Mesh *hinges = new Mesh;
	gen_cylinder(hinges, HINGE_RAD, HINGE_HEIGHT, 10, 1, 1);
	xform.set_rotation(Vector3(M_PI / 2.0, 0, 0));
	xform.translate(Vector3(0, VSIZE / 4.0, 0));
	hinges->apply_xform(xform);

	gen_cylinder(&tmp, HINGE_RAD, HINGE_HEIGHT, 10, 1, 1);
	xform.set_rotation(Vector3(M_PI / 2.0, 0, 0));
	xform.translate(Vector3(0, -VSIZE / 4.0, 0));
	tmp.apply_xform(xform);

	hinges->append(tmp);

	Object *ohinges = new Object;
	ohinges->set_mesh(hinges);
	obj.push_back(ohinges);


	return true;
}
