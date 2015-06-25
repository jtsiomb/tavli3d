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

static void display();
static void reshape(int x, int y);
static void keypress(unsigned char key, int x, int y);
static void keyrelease(unsigned char key, int x, int y);
static void mouse(int bn, int st, int x, int y);
static void motion(int x, int y);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1280, 800);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutCreateWindow("Tavli");

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
	game_keyboard(key, true);
}

static void keyrelease(unsigned char key, int x, int y)
{
	game_keyboard(key, false);
}

static void mouse(int bn, int st, int x, int y)
{
	game_mbutton(bn - GLUT_LEFT_BUTTON, st == GLUT_DOWN, x, y);
}

static void motion(int x, int y)
{
	game_mmotion(x, y);
}
