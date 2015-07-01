#include <stdio.h>
#include <assert.h>
#include "opengl.h"
#include "game.h"
#include "board.h"
#include "scenery.h"
#include "sdr.h"
#include "shadow.h"
#include "opt.h"

static void draw_scene();
static void draw_backdrop();

int win_width, win_height;
unsigned long cur_time;
unsigned int sdr_phong, sdr_phong_notex;
unsigned int sdr_shadow, sdr_shadow_notex;
unsigned int sdr_unlit;
bool wireframe;
int dbg_int;

static Board board;

static float cam_theta, cam_phi = 45, cam_dist = 3;
static bool bnstate[8];
static int prev_x, prev_y;

static bool dbg_busyloop, dbg_show_shadowmap;

bool game_init()
{
	if(init_opengl() == -1) {
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float amb[] = {0.3, 0.3, 0.3, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	if(glcaps.sep_spec) {
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	}

	if(glcaps.fsaa) {
		glEnable(GL_MULTISAMPLE);
	}

	if(glcaps.shaders) {
		Mesh::use_custom_sdr_attr = false;
		if(!(sdr_phong = create_program_load("sdr/phong.v.glsl", "sdr/phong.p.glsl"))) {
			return false;
		}
		if(!(sdr_phong_notex = create_program_load("sdr/phong.v.glsl", "sdr/phong-notex.p.glsl"))) {
			return false;
		}

		if(glcaps.fbo) {
			init_shadow(2048);

			if(!(sdr_shadow = create_program_load("sdr/shadow.v.glsl", "sdr/shadow.p.glsl"))) {
				return false;
			}
			set_uniform_int(sdr_shadow, "tex", 0);
			set_uniform_int(sdr_shadow, "shadowmap", 1);

			if(!(sdr_shadow_notex = create_program_load("sdr/shadow.v.glsl", "sdr/shadow-notex.p.glsl"))) {
				return false;
			}
			set_uniform_int(sdr_shadow_notex, "shadowmap", 1);
		}
	}

	if(!board.init()) {
		return false;
	}
	board.setup();

	if(!init_scenery()) {
		return false;
	}

	assert(glGetError() == GL_NO_ERROR);
	return true;
}

void game_cleanup()
{
	board.destroy();
	destroy_scenery();
	destroy_shadow();
}

void game_update(unsigned long time_msec)
{
	cur_time = time_msec;
}

void game_display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0.1, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);

	float lpos[] = {-10, 20, 10, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	if(opt.shadows && sdr_shadow) {
		begin_shadow_pass(Vector3(lpos[0], lpos[1], lpos[2]), Vector3(0, 0, 0), 4.5);
		draw_scene();
		end_shadow_pass();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, get_shadow_tex());

		glMatrixMode(GL_TEXTURE);
		Matrix4x4 shadow_matrix = get_shadow_matrix();
		glLoadTransposeMatrixf(shadow_matrix[0]);

		glActiveTexture(GL_TEXTURE0);
		glMatrixMode(GL_MODELVIEW);

		draw_scene();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		draw_scene();
	}

	if(dbg_show_shadowmap) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glPushAttrib(GL_ENABLE_BIT);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, get_shadow_tex());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);

		glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 1);
		glTexCoord2f(0, 0); glVertex2f(-1, -1);
		glTexCoord2f(1, 0); glVertex2f(1, -1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(0, 1); glVertex2f(-1, 1);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	if(dbg_busyloop) {
		redisplay();
	}
}

static void draw_scene()
{
	draw_backdrop();
	draw_scenery();
	board.draw();
}

static void draw_backdrop()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(0.9, 0.8, 0.6);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glColor3f(0.4, 0.5, 0.8);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void game_reshape(int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)x / (float)y, 0.2, 200.0);

	glViewport(0, 0, x, y);
}

void game_keyboard(int bn, bool press)
{
	if(press) {
		switch(bn) {
		case 27:
			quit();

		case 'w':
			wireframe = !wireframe;
			redisplay();
			break;

		case 'd':
			dbg_busyloop = !dbg_busyloop;
			redisplay();
			break;

		case 'D':
			dbg_show_shadowmap = !dbg_show_shadowmap;
			redisplay();
			break;

		case 's':
			opt.shadows = !opt.shadows;
			redisplay();
			break;

		case '=':
			dbg_int++;
			printf("dbg_int: %d\n", dbg_int);
			redisplay();
			break;

		case '-':
			dbg_int--;
			printf("dbg_int: %d\n", dbg_int);
			redisplay();
			break;
		}
	}
}

void game_mbutton(int bn, bool press, int x, int y)
{
	bnstate[bn] = press;
	prev_x = x;
	prev_y = y;
}

void game_mmotion(int x, int y)
{
	int dx = x - prev_x;
	int dy = y - prev_y;
	prev_x = x;
	prev_y = y;

	if(bnstate[0]) {
		cam_theta += dx * 0.5;
		cam_phi += dy * 0.5;

		if(cam_phi < -90) cam_phi = -90;
		if(cam_phi > 90) cam_phi = 90;

		redisplay();
	}
	if(bnstate[2]) {
		cam_dist += dy * 0.1;
		if(cam_dist < 0.0) cam_dist = 0.0;

		redisplay();
	}
}
