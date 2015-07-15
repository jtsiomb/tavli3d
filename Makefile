PREFIX ?= /usr/local

src = $(wildcard src/*.cc) $(wildcard libs/vmath/*.cc)
csrc = $(wildcard src/*.c) $(wildcard libs/vmath/*.c) $(wildcard libs/imago/*.c)
obj = $(src:.cc=.o) $(csrc:.c=.o)
dep = $(obj:.o=.d)

bin = tavli

inc = -Ilibs
CFLAGS = -pedantic -Wall -g $(inc)
CXXFLAGS = -pedantic -Wall -g $(inc)
LDFLAGS = $(libgl) -lm -lpthread -ldl -lpng -ljpeg -lz

ifeq ($(shell uname -s), Darwin)
	libgl = -framework OpenGL -framework GLUT -lGLEW
else
	libgl = -lGL -lGLU -lglut -lGLEW
endif

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
