#ifndef GAME_H_
#define GAME_H_

extern int win_width, win_height;

extern bool wireframe;

bool game_init();
void game_cleanup();
void game_update(unsigned long time_msec);
void game_display();
void game_reshape(int x, int y);
void game_keyboard(int bn, bool press);
void game_mbutton(int bn, bool press, int x, int y);
void game_mmotion(int x, int y);

void redisplay();
void quit();

#endif	/* GAME_H_ */
