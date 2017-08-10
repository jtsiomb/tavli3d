#ifndef UI_H_
#define UI_H_

#include <stdarg.h>
#include <gmath/gmath.h>

void set_message_timeout(long timeout);
void show_message(const char *fmt, ...);
void show_message(long timeout, const Vec3 &color, const char *fmt, ...);
void show_messagev(long timeout, const Vec3 &color, const char *fmt, va_list ap);

void print_text(const Vec2 &pos, const Vec3 &color, const char *fmt, ...);
void print_textv(const Vec2 &pos, const Vec3 &color, const char *fmt, va_list ap);

void draw_ui();

#endif	// UI_H_
