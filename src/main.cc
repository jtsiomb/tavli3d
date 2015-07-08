#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "game.h"
#include "opt.h"

static void display();
static void reshape(int x, int y);
static void keypress(unsigned char key, int x, int y);
static void keyrelease(unsigned char key, int x, int y);
static void mouse(int bn, int st, int x, int y);
static void motion(int x, int y);
static void update_modifiers();


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	if(!init_options(argc, argv)) {
		return 1;
	}

	glutInitWindowSize(opt.xres, opt.yres);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutCreateWindow("Tavli");

	if(opt.fullscreen) {
		glutFullScreen();
	}

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyrelease);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);

	glewInit();

	if(!game_init()) {
		return 1;
	}
	atexit(game_cleanup);

	glutMainLoop();
	return 0;
}

void redisplay()
{
	glutPostRedisplay();
}

void quit()
{
	exit(0);
}

static void display()
{
	unsigned int msec = glutGet(GLUT_ELAPSED_TIME);
	game_update(msec);
	game_display();

	assert(glGetError() == GL_NO_ERROR);
	glutSwapBuffers();
}

static void reshape(int x, int y)
{
	win_width = x;
	win_height = y;

	game_reshape(x, y);
}

static void keypress(unsigned char key, int x, int y)
{
	update_modifiers();
	game_keyboard(key, true);
}

static void keyrelease(unsigned char key, int x, int y)
{
	update_modifiers();
	game_keyboard(key, false);
}

static void mouse(int bn, int st, int x, int y)
{
	update_modifiers();
	game_mbutton(bn - GLUT_LEFT_BUTTON, st == GLUT_DOWN, x, y);
}

static void motion(int x, int y)
{
	game_mmotion(x, y);
}

void update_modifiers()
{
	static unsigned int prev_mod;
	unsigned int mod = glutGetModifiers();
	unsigned int delta = mod ^ prev_mod;

	if(delta & GLUT_ACTIVE_SHIFT) {
		bool press = (mod & GLUT_ACTIVE_SHIFT) != 0;
		game_modifier_key(MOD_SHIFT, press);
	}
	if(delta & GLUT_ACTIVE_CTRL) {
		bool press = (mod & GLUT_ACTIVE_CTRL) != 0;
		game_modifier_key(MOD_CTL, press);
	}
	if(delta & GLUT_ACTIVE_ALT) {
		bool press = (mod & GLUT_ACTIVE_ALT) != 0;
		game_modifier_key(MOD_ALT, press);
	}

	prev_mod = mod;
}
