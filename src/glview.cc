#include <stdio.h>
#include <assert.h>
#include "opengl.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include "glview.h"
#include "game.h"
#include "timer.h"

static void update_modifiers();

static GLView *glview;

GLView::GLView()
{
	glview = this;
}

GLView::GLView(QWidget *w)
	: QOpenGLWidget(w)
{
	QSurfaceFormat fmt;
	fmt.setDepthBufferSize(24);
	fmt.setSamples(8);
	setFormat(fmt);

	glview = this;
}

GLView::~GLView()
{
	cleanup();
}

void GLView::initializeGL()
{
	if(!game_init()) {
		fprintf(stderr, "game init failed\n");
		quit();
	}
}

void GLView::cleanup()
{
	makeCurrent();
	game_cleanup();
}

void GLView::paintGL()
{
	unsigned long msec = get_time_msec();
	game_update(msec);
	game_display();

	assert(glGetError() == GL_NO_ERROR);
}

void GLView::resizeGL(int x, int y)
{
	win_width = x;
	win_height = y;

	game_reshape(x, y);
}

void GLView::keyPressEvent(QKeyEvent *ev)
{
	update_modifiers();
	game_keyboard(ev->key(), true);
}

void GLView::keyReleaseEvent(QKeyEvent *ev)
{
	update_modifiers();
	game_keyboard(ev->key(), false);
}

void GLView::mousePressEvent(QMouseEvent *ev)
{
	int bn;
	switch(ev->button()) {
	case Qt::LeftButton:
		bn = 0;
		break;
	case Qt::MidButton:
		bn = 1;
		break;
	case Qt::RightButton:
		bn = 2;
		break;
	default:
		return;
	}

	update_modifiers();
	game_mbutton(bn, true, ev->x(), ev->y());
}

void GLView::mouseReleaseEvent(QMouseEvent *ev)
{
	int bn;
	switch(ev->button()) {
	case Qt::LeftButton:
		bn = 0;
		break;
	case Qt::MidButton:
		bn = 1;
		break;
	case Qt::RightButton:
		bn = 2;
		break;
	default:
		return;
	}

	update_modifiers();
	game_mbutton(bn, false, ev->x(), ev->y());
}

void GLView::mouseMoveEvent(QMouseEvent *ev)
{
	game_mmotion(ev->x(), ev->y());
}

void GLView::wheelEvent(QWheelEvent *ev)
{
	// TODO
}

void redisplay()
{
	glview->update();
}

void quit()
{
	qApp->quit();
}

static void update_modifiers()
{
	static Qt::KeyboardModifiers prev_mod;
	Qt::KeyboardModifiers qmod = qApp->keyboardModifiers();
	Qt::KeyboardModifiers delta = qmod ^ prev_mod;

	if(delta & Qt::ShiftModifier) {
		bool press = (qmod & Qt::ShiftModifier) != 0;
		game_modifier_key(MOD_SHIFT, press);
	}
	if(delta & Qt::ControlModifier) {
		bool press = (qmod & Qt::ControlModifier) != 0;
		game_modifier_key(MOD_CTL, press);
	}
	if(delta & Qt::AltModifier) {
		bool press = (qmod & Qt::AltModifier) != 0;
		game_modifier_key(MOD_ALT, press);
	}

	prev_mod = qmod;
}
