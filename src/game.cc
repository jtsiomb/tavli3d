#include <stdio.h>
#include <GL/glew.h>
#include "game.h"
#include "board.h"

static void draw_backdrop();

int win_width, win_height;

static Board board;

static float cam_theta, cam_phi = 25, cam_dist = 6;
static bool bnstate[8];
static int prev_x, prev_y;


bool game_init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if(!board.init()) {
		return false;
	}

	return true;
}

void game_cleanup()
{
	board.destroy();
}

void game_update(unsigned long time_msec)
{
}

void game_display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);

	draw_backdrop();

	board.draw();

	/*
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-1, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, -1);
	glVertex3f(-1, 0, -1);
	glEnd();*/
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
	gluPerspective(50, (float)x / (float)y, 0.2, 200.0);

	glViewport(0, 0, x, y);
}

void game_keyboard(int bn, bool press)
{
	if(press) {
		switch(bn) {
		case 27:
			quit();
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
