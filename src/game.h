#ifndef GAME_H_
#define GAME_H_

#include <gmath/gmath.h>

extern int win_width, win_height;
extern unsigned int sdr_phong, sdr_phong_notex;
extern unsigned int sdr_shadow, sdr_shadow_notex;
extern unsigned int sdr_unlit;
extern long cur_time;

extern Ray pick_ray;

extern bool wireframe;
extern int dbg_int;

enum { MOD_ALT, MOD_CTL, MOD_SHIFT };

bool game_init();
void game_cleanup();
void game_update(unsigned long time_msec);
void game_display();
void game_reshape(int x, int y);
void game_keyboard(int key, bool press);
void game_modifier_key(int key, bool press);
void game_mbutton(int bn, bool press, int x, int y);
void game_mmotion(int x, int y);

void redisplay();
void quit();

#endif	/* GAME_H_ */
