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
	if(!generate_textures()) {
		return false;
	}
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
	Mesh tmp;
	Matrix4x4 xform;

	obj.clear();

	for(int i=0; i<2; i++) {
		int sign = i * 2 - 1;

		// generate bottom
		Mesh *bottom = new Mesh;
		gen_box(bottom, HSIZE, BOT_THICKNESS, HSIZE * 2.0);
		xform.set_translation(Vector3(0, -BOT_THICKNESS / 2.0, 0));
		bottom->apply_xform(xform);

		Object *obottom = new Object;
		obottom->set_mesh(bottom);
		obottom->xform().set_translation(Vector3(sign * (HSIZE / 2.0 + WALL_THICKNESS + HINGE_RAD * 0.25), 0, 0));
		obottom->set_texture(img_field.texture());
		obj.push_back(obottom);


		// generate the 4 sides
		Mesh *sides = new Mesh;
		gen_box(sides, WALL_THICKNESS, WALL_HEIGHT, VSIZE + WALL_THICKNESS * 2);
		xform.set_translation(Vector3(-(HSIZE + WALL_THICKNESS) / 2.0,
					WALL_HEIGHT / 2.0 - BOT_THICKNESS, 0));
		sides->apply_xform(xform);

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
		osides->xform() = obottom->xform();
		obj.push_back(osides);

	}


	// generate the hinges
	Mesh *hinges = new Mesh;
	gen_cylinder(hinges, HINGE_RAD, HINGE_HEIGHT, 10, 1, 1);
	xform.reset_identity();
	xform.translate(Vector3(0, WALL_HEIGHT - HINGE_RAD * 0.5, VSIZE / 4.0));
	xform.rotate(Vector3(M_PI / 2.0, 0, 0));
	hinges->apply_xform(xform);

	gen_cylinder(&tmp, HINGE_RAD, HINGE_HEIGHT, 10, 1, 1);
	xform.reset_identity();
	xform.translate(Vector3(0, WALL_HEIGHT - HINGE_RAD * 0.5, -VSIZE / 4.0));
	xform.rotate(Vector3(M_PI / 2.0, 0, 0));
	tmp.apply_xform(xform);

	hinges->append(tmp);

	Object *ohinges = new Object;
	ohinges->set_mesh(hinges);
	obj.push_back(ohinges);


	return true;
}

static bool spike(float x, float y)
{
	x = fmod(x * 5.0, 1.0);
	return y < (x < 0.5 ? 2.0 * x : 2.0 - 2.0 * x);
}

static bool circle(float x, float y, float rad)
{
	x = fmod(x * 5.0, 1.0) - 0.5;
	y = (y - 0.65) * 5.0;
	float len = sqrt(x * x + y * y);
	return len < rad;
}

static bool diamond(float x, float y)
{
	return y >= (1.0 - (x < 0.5 ? 2.0 * x : 2.0 - 2.0 * x)) * 0.3333333 + 0.88;
}

static bool center_circle(float x, float y, float rad)
{
	x = x - 0.5;
	y = 1.0 - y;
	return sqrt(x * x + y * y) < rad;
}

bool Board::generate_textures()
{
	const int xsz = 512;
	const int ysz = 1024;

	img_field.create(xsz, ysz);
	clear_image(&img_field, 0, 0, 0);

	unsigned char *pptr = img_field.pixels;

	for(int i=0; i<ysz; i++) {
		float v = (float)i / (float)ysz;

		for(int j=0; j<xsz; j++) {
			float u = (float)j / (float)xsz;

			int r = 0, g = 0, b = 0;

			float x = u;
			float y = v < 0.5 ? v * 2.0 : 2.0 - v * 2.0;
			bool inside = false;

			inside |= (spike(x, y + 0.33333) && !spike(x, y + 0.4)) ||
				(spike(x, y + 0.5) && !spike(x, y + 0.68));
			inside |= (circle(x, y, 0.12) && !circle(x, y, 0.1)) || circle(x, y, 0.06);
			inside |= (diamond(x, y) && !diamond(x, y - 0.015)) ||
				(diamond(x, y - 0.023) && !diamond(x, y - 0.028));
			inside |= center_circle(x, y, 0.03);

			if(inside) {
				r = g = b = 255;
			}

			pptr[0] = r;
			pptr[1] = g;
			pptr[2] = b;
			pptr += 3;
		}
	}

	img_field.texture();
	return true;
}
