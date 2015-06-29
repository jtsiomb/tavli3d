#include <vector>
#include "opengl.h"
#include "scenery.h"
#include "game.h"
#include "mesh.h"
#include "meshgen.h"
#include "object.h"
#include "revol.h"
#include "image.h"
#include "sdr.h"
#include "opt.h"

static bool gen_textures();

static std::vector<Object*> obj;
static Image img_marble;

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



bool init_scenery()
{
	if(!gen_textures()) {
		return false;
	}

	Matrix4x4 xform;

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
	obj.push_back(otable);


	// meshgen stats
	printf("Generated scenery:\n  %u meshes\n", (unsigned int)obj.size());
	unsigned int polycount = 0;
	for(size_t i=0; i<obj.size(); i++) {
		const Mesh *m = obj[i]->get_mesh();
		polycount += m->get_poly_count();
	}
	printf("  %u polygons\n", polycount);

	return true;
}

void destroy_scenery()
{
	for(size_t i=0; i<obj.size(); i++) {
		delete obj[i];
	}
	obj.clear();

	img_marble.destroy();
}

void draw_scenery()
{
	unsigned int sdr = opt.shadows && sdr_shadow ? sdr_shadow : sdr_phong;

	for(size_t i=0; i<obj.size(); i++) {
		if(wireframe) {
			obj[i]->draw_wire();
			obj[i]->draw_normals(0.075);
		} else {
			obj[i]->set_shader(sdr);
			obj[i]->draw();
		}
	}
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
			pptr += 3;
		}
	}
	img_marble.texture();

	return true;
}
