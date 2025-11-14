#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_PREFIX "emerg: "

void __pr_emerg(const char *fmt, ...)
{
	static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

	int r = 0;
	va_list vl;
	char buf[32];
	char vbuf[2048];
	const int ul = (int)sizeof(vbuf) - 4;

	va_start(vl, fmt);
	pthread_mutex_lock(&print_lock);

	do {
		r += snprintf(&vbuf[r], ul - r, " emerg");
		r += vsnprintf(&vbuf[r], ul - r, fmt, vl);
		vbuf[r++] = '\n';
		vbuf[r] = '\0';
	} while (0);

	r = (int)fwrite(vbuf, sizeof(char), (size_t)r, stdout);
	// gui_pr_queue_buffer(vbuf, (size_t)r);
	pthread_mutex_unlock(&print_lock);
	va_end(vl);
	(void)r;
}

void __va_test(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	va_end(vl);
}

int __get_time_str(char *buf, size_t n)
{
	time_t rawtime;
	struct tm *tm;

	time(&rawtime);
	tm = localtime(&rawtime);

	return snprintf(buf, n, "[%d:%d:%d]", tm->tm_hour, tm->tm_min,
			tm->tm_sec);
}

void __pr_func(const char *fmt, ...)
{
	int charoffset = 0;
	va_list ap;
	char buf[32]; /* for time str */
	char vbuf[2048];

	__get_time_str(buf, sizeof(buf) / sizeof(char));

	memset(vbuf, 0, sizeof(vbuf));

	va_start(ap, fmt);

	charoffset = charoffset + snprintf(&vbuf[charoffset], sizeof(vbuf) - 4,
					   " %s", buf);

	charoffset = charoffset + snprintf(&vbuf[charoffset], sizeof(vbuf) - 4,
					   " emerg: ");

	charoffset = charoffset +
		     vsnprintf(&vbuf[charoffset], sizeof(vbuf) - 4, fmt, ap);

	fwrite(vbuf, sizeof(char), charoffset, stdout);

	va_end(ap);
	// bt here
}

int main()
{
	int x = 9928;
	__pr_func("overflow bug %d", x);
}