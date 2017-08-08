#include <stdio.h>
#include <stdarg.h>
#include "logger.h"
#include "ui.h"

#if defined(unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#define UI_MSG_TIMEOUT 4000

enum { LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL, LOG_DEBUG };

static int typecolor(int type);

static FILE *fp = stdout;

static void logmsg(int type, const char *fmt, va_list ap)
{
#if defined(unix) || defined(__unix__) || (defined(__APPLE__) && !defined(TARGET_IPHONE))
	if(isatty(fileno(fp)) && type != LOG_INFO) {
		int c = typecolor(type);
		fprintf(fp, "\033[%dm", c);
		vfprintf(fp, fmt, ap);
		fprintf(fp, "\033[0m");
	} else
#endif
	{
		vfprintf(fp, fmt, ap);
	}
	if(type == LOG_ERROR || type == LOG_FATAL || type == LOG_DEBUG) {
		fflush(fp);
	}

#ifdef WIN32
	if(type == LOG_FATAL) {
		static char msgbuf[1024];
		vsnprintf(msgbuf, sizeof msgbuf - 1, fmt, ap);
		msgbuf[sizeof msgbuf - 1] = 0;
		MessageBox(0, msgbuf, "Fatal error", MB_OK | MB_ICONSTOP);
	}
#endif
}

extern "C" void info_log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logmsg(LOG_INFO, fmt, ap);
	va_end(ap);
}

extern "C" void warning_log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logmsg(LOG_WARNING, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	show_messagev(UI_MSG_TIMEOUT, Vec3(1.0, 0.8, 0.1), fmt, ap);
	va_end(ap);
}

extern "C" void error_log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logmsg(LOG_ERROR, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	show_messagev(UI_MSG_TIMEOUT, Vec3(1.0, 0.1, 0.1), fmt, ap);
	va_end(ap);
}

extern "C" void fatal_log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logmsg(LOG_FATAL, fmt, ap);
	va_end(ap);
}

extern "C" void debug_log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logmsg(LOG_DEBUG, fmt, ap);
	va_end(ap);
}

enum {
	BLACK = 0,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE
};

#define ANSI_FGCOLOR(x)	(30 + (x))
#define ANSI_BGCOLOR(x)	(40 + (x))

static int typecolor(int type)
{
	switch(type) {
	case LOG_ERROR:
		return ANSI_FGCOLOR(RED);
	case LOG_FATAL:
		return ANSI_FGCOLOR(RED);	// TODO differentiate from LOG_ERROR
	case LOG_WARNING:
		return ANSI_FGCOLOR(YELLOW);
	case LOG_DEBUG:
		return ANSI_FGCOLOR(MAGENTA);
	default:
		break;
	}
	return 37;
}
