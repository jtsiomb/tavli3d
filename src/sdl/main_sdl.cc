#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "opengl.h"
#include <SDL2/SDL.h>
#include "game.h"
#include "opt.h"

static void process_event(SDL_Event *ev);

static SDL_Window *win;
static SDL_GLContext ctx;
static bool redraw_pending = true;
static bool done;

int main(int argc, char **argv)
{
	if(!init_options(argc, argv)) {
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	int pos = SDL_WINDOWPOS_UNDEFINED;
	unsigned int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	if(opt.fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if(!(win = SDL_CreateWindow("tavli", pos, pos, opt.xres, opt.yres, flags))) {
		fprintf(stderr, "failed to create window\n");
		return 1;
	}
	if(!(ctx = SDL_GL_CreateContext(win))) {
		fprintf(stderr, "failed to create OpenGL context\n");
		return 1;
	}

	if(!game_init()) {
		return 1;
	}
	unsigned long start_time = SDL_GetTicks();

	SDL_GetWindowSize(win, &win_width, &win_height);
	game_reshape(win_width, win_height);

	for(;;) {
		SDL_Event ev;
		if(!redraw_pending) {
			SDL_WaitEvent(&ev);
			process_event(&ev);
			if(done) goto evloop_end;
		}
		while(SDL_PollEvent(&ev)) {
			process_event(&ev);
			if(done) goto evloop_end;
		}

		if(redraw_pending) {
			unsigned long msec = SDL_GetTicks() - start_time;
			game_update(msec);
			game_display();

			assert(glGetError() == GL_NO_ERROR);
			SDL_GL_SwapWindow(win);
		}
	}

evloop_end:
	game_cleanup();
	SDL_Quit();
	return 0;
}

void redisplay()
{
	redraw_pending = true;
}

void quit()
{
	done = true;
}

static void process_event(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_QUIT:
		done = true;
		break;

	case SDL_WINDOWEVENT:
		switch(ev->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			win_width = ev->window.data1;
			win_height = ev->window.data2;
			game_reshape(win_width, win_height);
			break;

		case SDL_WINDOWEVENT_EXPOSED:
			redraw_pending = true;
			break;

		default:
			break;
		}
		break;

	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch(ev->key.keysym.sym) {
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			game_modifier_key(MOD_SHIFT, ev->key.state == SDL_PRESSED);
			break;

		case SDLK_LALT:
		case SDLK_RALT:
			game_modifier_key(MOD_ALT, ev->key.state == SDL_PRESSED);
			break;

		case SDLK_RCTRL | SDLK_LCTRL:
			game_modifier_key(MOD_CTL, ev->key.state == SDL_PRESSED);
			break;

		default:
			game_keyboard(ev->key.keysym.sym, ev->key.state == SDL_PRESSED);
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		game_mbutton(ev->button.button - SDL_BUTTON_LEFT, ev->button.state == SDL_PRESSED,
				ev->button.x, ev->button.y);
		break;

	case SDL_MOUSEMOTION:
		game_mmotion(ev->motion.x, ev->motion.y);
		break;

	default:
		break;
	}
}
