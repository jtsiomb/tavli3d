#ifndef GLVIEW_H
#define GLVIEW_H

#include <QOpenGLWidget>

class GLView : public QOpenGLWidget {
public:
	GLView();
	GLView(QWidget *w);
	virtual ~GLView();

	virtual void initializeGL();
	void cleanup();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

	virtual void keyPressEvent(QKeyEvent *ev);
	virtual void keyReleaseEvent(QKeyEvent *ev);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev);
	virtual void wheelEvent(QWheelEvent *ev);
};

void post_redisplay();

#endif // GLVIEW_H
