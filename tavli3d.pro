QT += core gui widgets opengl

CONFIG += debug

TARGET = tavli3d
TEMPLATE = app

SOURCES += src/opengl.c src/sdr.c src/board.cc src/game.cc src/geom.cc \
	src/glview.cc src/image.cc src/light.cc src/logger.cc src/main.cc \
	src/mesh.cc src/meshgen.cc src/object.cc src/opt.cc src/pnoise.cc \
	src/revol.cc src/scene.cc src/scenery.cc src/shadow.cc src/snode.cc \
	src/glui.cc src/mainwin.cc src/timer.cc
HEADERS += src/board.h src/game.h src/geom.h src/glview.h src/image.h \
	src/light.h src/logger.h src/meshgen.h src/mesh.h src/object.h \
	src/opengl.h src/opt.h src/pnoise.h src/revol.h src/scene.h \
	src/scenery.h src/sdr.h src/shadow.h src/snode.h src/glui.h \
	src/mainwin.h src/timer.h

INCLUDEPATH += src
LIBS += -lm -lpthread -ldl -lpng -ljpeg -lz -lgmath -limago -ldrawtext -lGLU

FORMS += ui/mainwin.ui
