PREFIX ?= /usr/local
#sys = glut
sys = sdl

src = $(wildcard src/*.cc) $(wildcard src/$(sys)/*.cc)
csrc = $(wildcard src/*.c) $(wildcard src/$(sys)/*.c)
obj = $(src:.cc=.o) $(csrc:.c=.o)
dep = $(obj:.o=.d)

bin = tavli

CFLAGS = -pedantic -Wall -g -Isrc $(inc$(sys))
CXXFLAGS = -pedantic -Wall -g -Isrc $(inc$(sys))
LDFLAGS = $(libgl) $(lib$(sys)) -lm -lpthread -ldl -lpng -ljpeg -lz -lgmath -limago -ldrawtext -lvmath

ifeq ($(shell uname -s), Darwin)
	libgl = -framework OpenGL -lGLEW
	libglut = -framework GLUT
else
	libgl = -lGL -lGLU -lGLEW
	libglut = -lglut
endif

incsdl = `pkg-config --cflags sdl2`
libsdl = `pkg-config --libs sdl2`

$(bin): $(obj)
	$(CXX) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.cc
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

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
