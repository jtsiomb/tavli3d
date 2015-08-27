#include <vector>
#include "opengl.h"
#include "scenery.h"
#include "game.h"
#include "mesh.h"
#include "meshgen.h"
#include "object.h"
#include "scene.h"
#include "revol.h"
#include "image.h"
#include "sdr.h"
#include "opt.h"

static bool gen_textures();

static Scene scn;
static Image img_marble;
static Image img_coffee;

static const vec2_t table_cp[] = {
	{0, 0},
	{3, 0},	// mid 0
	{5.8, 0},
	{5.99, 0},	// mid 1
	{6, -0.1},
	{6.1, -0.1},	// mid 2
	{6.13, -0.3},
	{3, -0.3},	// mid 3
	{0, -0.3}
};
static const BezCurve table_curve = {
	sizeof table_cp / sizeof *table_cp,
	(vec2_t*)table_cp,
	1.0 / 6.2
};

static const vec2_t glass_cp[] = {
	{0.0, 0.4},
	{0.4, 0.4}, // mid 0 (inner bottom)
	{1.1, 0.4},
	{1.2, 0.4},	// mid 1 (inner bottom fillet)
	{1.3, 0.5},
	{1.8, 3.0},	// mid 2 (inside)
	{1.8, 6.0},
	{1.8, 6.2}, // mid 3 (top inner curve)
	{1.9, 6.26},
	{2.0, 6.2},	// mid 4 (top outer curve)
	{2.0, 6.0},
	{2.0, 3.5},	// mid 5 (outside)
	{1.5, 0.4},
	{1.4, 0.0},	// mid 6 (bottom fillet)
	{1.2, 0.0},
	{0.5, 0.0},	// mid 7 (bottom)
	{0.0, 0.0}
};
static const BezCurve glass_curve = {
	sizeof glass_cp / sizeof *glass_cp,
	(vec2_t*)glass_cp,
	0.075
};

static const vec2_t coffee_cp[] = {
	{0.0, 5.55},
	{0.9, 5.55}, // mid 0 (top)
	{1.7, 5.55},
	{1.8, 5.55}, // mid 1 (top fillet)
	{1.8, 4.5},
	{1.8, 2.5},	// mid 2 (inside)
	{1.3, 0.5},
	{1.2, 0.4},	// mid 3 (bottom fillet)
	{1.1, 0.4},
	{0.4, 0.4}, // mid 4 (bottom)
	{0.0, 0.4}
};
static const BezCurve coffee_curve = {
	sizeof coffee_cp / sizeof *coffee_cp,
	(vec2_t*)coffee_cp,
	0.078
};


bool init_scenery()
{
	if(!gen_textures()) {
		return false;
	}

	Matrix4x4 xform;

	// --- generate the table ---
	Mesh *table = new Mesh;
	gen_revol(table, 48, 16, bezier_revol, bezier_revol_normal, (void*)&table_curve);
	table->texcoord_gen_plane(Vector3(0, 1, 0), Vector3(1, 0, 0));
	xform.set_scaling(Vector3(0.5, 0.5, 0.5));
	xform.translate(Vector3(1, 1, 0));
	table->texcoord_apply_xform(xform);

	static const float table_scale = 1.8;
	xform.set_scaling(Vector3(table_scale, table_scale, table_scale));
	table->apply_xform(xform);

	Object *otable = new Object;
	otable->set_mesh(table);
	otable->mtl.diffuse = Vector3(1, 1, 1);
	otable->mtl.specular = Vector3(0.7, 0.7, 0.7);
	otable->xform().set_translation(Vector3(0, -0.025, 0));
	otable->set_texture(img_marble.texture());
	scn.add_object(otable);

	/*
	// --- generate the frappe glass ---
	Mesh *glass = new Mesh;
	gen_revol(glass, 24, 24, bezier_revol, bezier_revol_normal, (void*)&glass_curve);

	Object *oglass = new Object;
	oglass->set_mesh(glass);
	oglass->set_shader(0);
	oglass->rop.cast_shadows = false;
	oglass->rop.transparent = true;
	oglass->mtl.alpha = 0.3;
	scn.add_object(oglass);

	// --- generate the coffee liquid ---
	Mesh *coffee = new Mesh;
	gen_revol(coffee, 24, 12, bezier_revol, bezier_revol_normal, (void*)&coffee_curve);

	Object *ocoffee = new Object;
	ocoffee->set_mesh(coffee);
	ocoffee->set_shader(0);
	//ocoffee->mtl.diffuse = Vector3(0.42, 0.28, 0.15);
	ocoffee->set_texture(img_coffee.texture());
	scn.add_object(ocoffee);
	*/

	// meshgen stats
	int nobj = (int)scn.objects.size();
	printf("Generated scenery:\n  %d meshes\n", nobj);
	unsigned int polycount = 0;
	for(int i=0; i<nobj; i++) {
		const Mesh *m = scn.objects[i]->get_mesh();
		polycount += m->get_poly_count();
	}
	printf("  %u polygons\n", polycount);

	return true;
}

void destroy_scenery()
{
	scn.clear();
	img_marble.destroy();
}

void draw_scenery(unsigned int flags)
{
	scn.draw(flags);
}

static float marble(float x, float y)
{
	float theta = x * M_PI * 2.0 * cos(y * 1.5);
	theta += turbulence2(x * 10.0, y * 10.0, 4) * 2;
	float val = 0.5 + sin(theta * 5.0) + sin(theta * 8.0) / 2.0 + sin(theta * 19.0) / 4.0;
	return val * 0.5 + 0.5;
}

static bool gen_textures()
{
	static const Vector3 marble_col1 = Vector3(0.78, 0.85, 0.85);
	static const Vector3 marble_col2 = Vector3(0.56, 0.68, 0.7);

	img_marble.create(512, 512);
	unsigned char *pptr = img_marble.pixels;
	for(int i=0; i<img_marble.height; i++) {
		float v = (float)i / (float)img_marble.height;
		for(int j=0; j<img_marble.width; j++) {
			float u = (float)j / (float)img_marble.width;

			float marble_val = marble(u, v);
			Vector3 color = lerp(marble_col2, marble_col1, marble_val) * 0.6;

			int r = (int)(color.x * 255.0);
			int g = (int)(color.y * 255.0);
			int b = (int)(color.z * 255.0);

			pptr[0] = r > 255 ? 255 : (r < 0 ? 0 : r);
			pptr[1] = g > 255 ? 255 : (g < 0 ? 0 : g);
			pptr[2] = b > 255 ? 255 : (b < 0 ? 0 : b);
			pptr[3] = 255;
			pptr += 4;
		}
	}
	img_marble.texture();

	// coffee texture
	static const Vector3 coffee_col = Vector3(0.42, 0.28, 0.15);
	static const Vector3 foam_col = Vector3(0.81, 0.7, 0.52);

	img_coffee.create(256, 256);
	pptr = img_coffee.pixels;
	for(int i=0; i<img_coffee.height; i++) {
		float v = (float)i / (float)img_coffee.height;
		for(int j=0; j<img_coffee.width; j++) {
			float u = (float)j / (float)img_coffee.width;

			Vector3 col = v < 0.4 ? foam_col : coffee_col;

			int r, g, b;
			r = col.x * 255.0;
			g = col.y * 255.0;
			b = col.z * 255.0;

			pptr[0] = r > 255 ? 255 : (r < 0 ? 0 : r);
			pptr[1] = g > 255 ? 255 : (g < 0 ? 0 : g);
			pptr[2] = b > 255 ? 255 : (b < 0 ? 0 : b);
			pptr[3] = 255;
			pptr += 4;
		}
	}


	return true;
}
