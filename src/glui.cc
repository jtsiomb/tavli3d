#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gmath/gmath.h>
#include <drawtext.h>
#include "opengl.h"
#include "glui.h"
#include "game.h"

#define FONTSZ	16

static bool init();

struct Message {
	long start_time, show_until;
	char *str;
	Vec3 color;
	Message *next;
};
static Message *msglist;

struct Text {
	char *str;
	Vec2 pos;
	Vec3 color;
	Text *next;
};
static Text *txlist;

static long timeout = 2000;
static long trans_time = 250;
static dtx_font *font;

void set_message_timeout(long tm)
{
	timeout = tm;
}

void show_message(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	show_messagev(timeout, Vec3(1, 1, 1), fmt, ap);
	va_end(ap);
}

void show_message(long timeout, const Vec3 &color, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	show_messagev(timeout, color, fmt, ap);
	va_end(ap);
}

void show_messagev(long timeout, const Vec3 &color, const char *fmt, va_list ap)
{
	char buf[512];

	init();

	vsnprintf(buf, sizeof buf, fmt, ap);

	Message *msg = new Message;
	int len = strlen(buf);
	msg->str = new char[len + 1];
	memcpy(msg->str, buf, len + 1);
	msg->start_time = cur_time;
	msg->show_until = cur_time + timeout;
	msg->color = color;

	Message dummy;
	dummy.next = msglist;
	Message *prev = &dummy;
	while(prev->next && prev->next->show_until < msg->show_until) {
		prev = prev->next;
	}
	msg->next = prev->next;
	prev->next = msg;
	msglist = dummy.next;
}

void print_text(const Vec2 &pos, const Vec3 &color, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print_textv(pos, color, fmt, ap);
	va_end(ap);
}

void print_textv(const Vec2 &pos, const Vec3 &color, const char *fmt, va_list ap)
{
	char buf[512];

	init();

	vsnprintf(buf, sizeof buf, fmt, ap);

	Text *tx = new Text;
	int len = strlen(buf);
	tx->str = new char[len + 1];
	memcpy(tx->str, buf, len + 1);
	tx->color = color;
	tx->pos = Vec2(pos.x, -pos.y);

	tx->next = txlist;
	txlist = tx;
}

void draw_ui()
{
	if(!font) return;

	while(msglist && msglist->show_until <= cur_time) {
		Message *msg = msglist;
		msglist = msg->next;
		delete [] msg->str;
		delete msg;
	}

	dtx_use_font(font, FONTSZ);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, win_width, -win_height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(0);

	Message *msg = msglist;
	while(msg) {
		long t = cur_time - msg->start_time;
		long dur = msg->show_until - msg->start_time;
		float alpha = smoothstep(0, trans_time, t) *
			(1.0 - smoothstep(dur - trans_time, dur, t));
		glColor4f(msg->color.x, msg->color.y, msg->color.z, alpha);
		glTranslatef(0, -dtx_line_height(), 0);
		dtx_string(msg->str);
		msg = msg->next;
	}

	while(txlist) {
		Text *tx = txlist;
		txlist = txlist->next;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(tx->pos.x, tx->pos.y, 0);

		glColor3f(tx->color.x, tx->color.y, tx->color.z);
		dtx_string(tx->str);

		delete [] tx->str;
		delete tx;
	}

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static bool init()
{
	static bool done_init;
	if(done_init) return true;

	done_init = true;

	if(!(font = dtx_open_font("data/ui.font", 0))) {
		fprintf(stderr, "failed to open font: data/ui.font\n");
		return false;
	}
	dtx_prepare_range(font, FONTSZ, 32, 127);
	return true;
}
