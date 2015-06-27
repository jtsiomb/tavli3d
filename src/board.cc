#include <float.h>
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
#define PIECE_RAD		(0.45 * HSIZE / 5.0)

struct BezCurve {
	int numcp;
	vec2_t *cp;
	float scale;
};

static const vec2_t piece_cp[] = {
		{0, 0.25},
		{1, 0.25},	// mid0
		{2, 0.5},
		{2.5, 0.5},	// mid1
		{3, 0.5},
		{4, 0.5},	// mid2
		{4, 0},
		{4, -0.5},	// mid3
		{3, -0.5},
		{2.5, -0.5}, // mid4
		{0, -0.5}
};
static const BezCurve piece_curve = {sizeof piece_cp / sizeof *piece_cp, (vec2_t*)piece_cp, 0.25 * PIECE_RAD};


static Vector2 piece_revol(float u, float v, void *cls)
{
	BezCurve *curve = (BezCurve*)cls;
	int nseg = (curve->numcp - 1) / 2;

	if(v >= 1.0) v = 1.0 - 1e-6;
	int cidx = std::min((int)(v * nseg), nseg - 1);
	float t = fmod(v * (float)nseg, 1.0);

	const vec2_t *cp = curve->cp + cidx * 2;

	float resx = bezier(cp[0].x, cp[1].x, cp[1].x, cp[2].x, t);
	float resy = bezier(cp[0].y, cp[1].y, cp[1].y, cp[2].y, t);
	return Vector2(resx * curve->scale, resy * curve->scale);
}

static Vector2 piece_revol_normal(float u, float v, void *cls)
{
	BezCurve *curve = (BezCurve*)cls;
	int nseg = (curve->numcp - 1) / 2;

	if(v >= 1.0) v = 1.0 - 1e-6;
	int cidx = std::min((int)(v * nseg), nseg - 1);
	float t = fmod(v * (float)nseg, 1.0);

	const vec2_t *cp = curve->cp + cidx * 2;
	Vector2 cp0 = cp[0];
	Vector2 cp1 = cp[1];
	Vector2 cp2 = cp[2];

	Vector2 pprev, pnext;
	for(int i=0; i<2; i++) {
		pprev[i] = bezier(cp0[i], cp1[i], cp1[i], cp2[i], t - 0.05);
		pnext[i] = bezier(cp0[i], cp1[i], cp1[i], cp2[i], t + 0.05);
	}

	float tx = pnext.x - pprev.x;
	float ty = pnext.y - pprev.y;

	return Vector2(-ty, tx);
}

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
	for(int i=0; i<2; i++) {
		float sign = i * 2 - 1;

		// barrel
		gen_cylinder(&tmp, HINGE_RAD, HINGE_HEIGHT, 8, 1, 1);
		xform.reset_identity();
		xform.translate(Vector3(0, WALL_HEIGHT - HINGE_RAD * 0.5, sign * VSIZE / 4.0));
		xform.rotate(Vector3(-M_PI / 2.0, 0, 0));
		tmp.apply_xform(xform);
		hinges->append(tmp);

		// flange
		gen_plane(&tmp, HINGE_HEIGHT * 0.6, HINGE_HEIGHT * 0.8);
		tmp.apply_xform(xform);

		Matrix4x4 tex_xform;
		tex_xform.set_rotation(Vector3(0, 0, M_PI / 2.0));
		tmp.texcoord_apply_xform(tex_xform);
		hinges->append(tmp);

		// studs
		for(int j=0; j<4; j++) {
			Vector3 pos;

			pos.x = (float)((j & 1) * 2 - 1) * HINGE_HEIGHT * 0.2;
			pos.y = (float)((j & 2) - 1) * HINGE_HEIGHT * 0.3;

			Matrix4x4 stud_xform = xform;
			stud_xform.translate(pos);

			Matrix4x4 squash;
			squash.set_scaling(Vector3(1, 1, 0.5));

			gen_sphere(&tmp, HINGE_RAD * 0.5, 8, 4);
			tmp.apply_xform(stud_xform * squash);
			hinges->append(tmp);
		}
	}

	Object *ohinges = new Object;
	ohinges->set_mesh(hinges);
	ohinges->set_texture(img_hinge.texture());
	obj.push_back(ohinges);

	// debug object
	/*
	Mesh *dbgmesh = new Mesh;
	gen_box(dbgmesh, 0.5, 0.5, 0.5);
	xform.set_translation(Vector3(0, 0.4, 0));
	xform.set_scaling(Vector3(1, 1, 1));
	dbgmesh->apply_xform(xform);
	Object *dbgobj = new Object;
	dbgobj->set_mesh(dbgmesh);
	dbgobj->set_texture(img_hinge.texture());
	//dbgobj->tex_xform().set_scaling(Vector3(3, 3, 3));
	obj.push_back(dbgobj);
	*/

	Mesh *piece = new Mesh;
	gen_revol(piece, 18, 17, piece_revol, piece_revol_normal, (void*)&piece_curve);

	Object *opiece = new Object;
	opiece->set_mesh(piece);
	opiece->mtl.diffuse = Vector3(0.6, 0.6, 0.6);
	opiece->mtl.specular = Vector3(0.8, 0.8, 0.8);
	opiece->xform().set_translation(Vector3(0, 0.2, 0));
	obj.push_back(opiece);


	// meshgen stats
	printf("Generated board:\n  %u meshes\n", (unsigned int)obj.size());
	unsigned int polycount = 0;
	for(size_t i=0; i<obj.size(); i++) {
		const Mesh *m = obj[i]->get_mesh();
		polycount += m->get_poly_count();
	}
	printf("  %u polygons\n", polycount);

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

	img_field.create(1024, 1024);
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

	// ---- metal hinge diffuse texture ----
	Vector3 rusty_col1 = Vector3(0.43, 0.46, 0.52);
	Vector3 rusty_col2 = Vector3(0.52, 0.47, 0.43);

	img_hinge.create(128, 128);
	pptr = img_hinge.pixels;
	for(int i=0; i<img_hinge.height; i++) {
		float v = (float)i / (float)img_hinge.height;
		for(int j=0; j<img_hinge.width; j++) {
			float u = (float)j / (float)img_hinge.width;

			// rust pattern
			float w1 = fbm2(u * 4.0, v * 4.0, 3) * 0.5 + 0.5;
			//float w2 = fbm2(u * 8.0, v * 8.0, 1) * 0.5 + 0.5;
			Vector3 col = lerp(rusty_col1, rusty_col2 * 0.5, w1);

			// center hinge split
			if(fabs(v - 0.5) < 0.01) {
				col *= 0.5;
			}

			int r = (int)(col.x * 255.0);
			int g = (int)(col.y * 255.0);
			int b = (int)(col.z * 255.0);

			pptr[0] = r > 255 ? 255 : (r < 0 ? 0 : r);
			pptr[1] = g > 255 ? 255 : (g < 0 ? 0 : g);
			pptr[2] = b > 255 ? 255 : (b < 0 ? 0 : b);

			pptr += 3;
		}
	}
	img_hinge.texture();

	return true;
}
