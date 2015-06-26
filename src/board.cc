#include "opengl.h"
#include "board.h"
#include "meshgen.h"
#include "pnoise.h"

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

		// generate texture coordinates
		sides->texcoord_gen_box();

		Object *osides = new Object;
		osides->set_mesh(sides);
		osides->xform() = obottom->xform();
		osides->set_texture(img_wood.texture());
		osides->tex_xform().set_scaling(Vector3(2, 2, 2));
		osides->tex_xform().rotate(-Vector3(1, 0, 0.5), M_PI / 4.0);
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

	// debug object
	/*Mesh *dbgmesh = new Mesh;
	gen_box(dbgmesh, 0.5, 0.5, 0.5);
	xform.set_translation(Vector3(0, 0.4, 0));
	xform.set_scaling(Vector3(4, 1, 4));
	dbgmesh->apply_xform(xform);
	Object *dbgobj = new Object;
	dbgobj->set_mesh(dbgmesh);
	dbgobj->set_texture(img_wood.texture());
	dbgobj->tex_xform().set_scaling(Vector3(3, 3, 3));
	obj.push_back(dbgobj);*/

	return true;
}

static float wood(float x, float y)
{
	float u = x;
	float v = y;
	x += 1.0;
	x *= 10.0;
	y *= 20.0;

	float len = sqrt(x * x + y * y) + turbulence2(u * 6.0, v * 12.0, 2) * 1.2 +
		turbulence2(u * 0.5, v, 2) * 15.0;
	float val = fmod(len, 1.0);

	//val = val * 0.5 + 0.5;
	return val < 0.0 ? 0.0 : (val > 1.0 ? 1.0 : val);
}

static float wood_tile(float x, float y)
{
	float u = x;
	float v = y;
	x *= 10.0;
	y *= 10.0;

	float val = x + pnoise2(u * 6.0, v, 6, 1) * 3.0 +
		pturbulence2(u * 4, v * 2, 4, 2, 2) * 1.5 + pturbulence2(u * 8, v * 8, 8, 8, 2) * 0.5;

	val = fmod(val, 1.0);
	return val < 0.0 ? 0.0 : (val > 1.0 ? 1.0 : val);
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
	// ---- board field texture ----
	static const Vector3 wcol1 = Vector3(0.6, 0.4, 0.2);
	static const Vector3 wcol2 = Vector3(0.53, 0.32, 0.1);
	static const Vector3 wcol3 = Vector3(0.38, 0.25, 0.08);

	img_field.create(512, 1024);
	unsigned char *pptr = img_field.pixels;
	for(int i=0; i<img_field.height; i++) {
		float v = (float)i / (float)img_field.height;

		for(int j=0; j<img_field.width; j++) {
			float u = (float)j / (float)img_field.width;

			int r = 0, g = 0, b = 0;

			float wood_val = wood(u, v);

			// pattern mask
			float x = u;
			float y = v < 0.5 ? v * 2.0 : 2.0 - v * 2.0;
			bool inside = false;

			inside |= (spike(x, y + 0.33333) && !spike(x, y + 0.4)) ||
				(spike(x, y + 0.5) && !spike(x, y + 0.68));
			inside |= (circle(x, y, 0.12) && !circle(x, y, 0.1)) || circle(x, y, 0.06);
			inside |= (diamond(x, y) && !diamond(x, y - 0.015)) ||
				(diamond(x, y - 0.023) && !diamond(x, y - 0.028));
			inside |= center_circle(x, y, 0.03);

			Vector3 wood_color = lerp(wcol1, wcol2, wood_val) * 0.9;
			if(inside) {
				wood_color = lerp(wcol1, wcol2, 1.0 - wood_val) * 2.0;
			}

			r = (int)(wood_color.x * 255.0);
			g = (int)(wood_color.y * 255.0);
			b = (int)(wood_color.z * 255.0);

			pptr[0] = r > 255 ? 255 : r;
			pptr[1] = g > 255 ? 255 : g;
			pptr[2] = b > 255 ? 255 : b;
			pptr += 3;
		}
	}
	img_field.texture();

	// ---- generic wood texture ----
	img_wood.create(256, 256);
	pptr = img_wood.pixels;
	for(int i=0; i<img_wood.height; i++) {
		float v = (float)i / (float)img_wood.height;
		for(int j=0; j<img_wood.width; j++) {
			float u = (float)j / (float)img_wood.width;

			float wood_val = wood_tile(u, v);
			Vector3 wood_color = lerp(wcol2, wcol3, wood_val) * 0.7;

			int r = (int)(wood_color.x * 255.0);
			int g = (int)(wood_color.y * 255.0);
			int b = (int)(wood_color.z * 255.0);

			pptr[0] = r > 255 ? 255 : r;
			pptr[1] = g > 255 ? 255 : g;
			pptr[2] = b > 255 ? 255 : b;
			pptr += 3;
		}
	}
	img_wood.texture();
	return true;
}
