PREFIX ?= /usr/local
sys = glut
#sys = sdl

src = $(wildcard src/*.cc) $(wildcard src/$(sys)/*.cc) \
	  $(wildcard libs/gph-math/*.cc)
csrc = $(wildcard src/*.c) $(wildcard src/$(sys)/*.c) \
	   $(wildcard libs/miniglut/*.c) $(wildcard libs/glew/*.c)
obj = $(src:.cc=.o) $(csrc:.c=.o)
dep = $(obj:.o=.d)

bin = tavli

inc = -Isrc $(inc$(sys)) -Ilibs/miniglut -Ilibs/glew -Ilibs/gph-math
def = -DGLEW_STATIC -DMINIGLUT_USE_LIBC

CFLAGS = -pedantic -Wall -g $(inc) $(def) -MMD
CXXFLAGS = -pedantic -Wall -g $(inc) $(def) -MMD
LDFLAGS = $(libgl) $(lib$(sys)) -lm

ifeq ($(shell uname -s), Darwin)
	libgl = -framework OpenGL
	libglut = -framework GLUT
else
	libgl = -lGL -lGLU
	libglut = -lX11
endif

incsdl = `pkg-config --cflags sdl2`
libsdl = `pkg-config --libs sdl2`

$(bin): $(obj)
	$(CXX) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: install
install: $(bin)
	mkdir -p $(PREFIX)/bin
	cp $(bin) $(PREFIX)/bin/$(bin)

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/$(bin)
