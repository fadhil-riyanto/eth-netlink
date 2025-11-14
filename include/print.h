#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>

#if defined(__linux__) && defined(__unix__)
#include <pthread.h>
	static pthread_mutex_t print_fast_mu = PTHREAD_MUTEX_INITIALIZER;
#endif

// #define PREPROCESSOR_DEFINE #define

/* todo: create a hack without define trampoline repeatly */

#define DEFINE_HDR_PR_FUNCTION(NAME) 			\
	void __##NAME(const char *fmt, ...);	
	// \
	// PREPROCESSOR_DEFINE NAME __NAME

#define DEFINE_PR_FUNCTION(NAME, LOG_PREFIX)                                \
	void __##NAME(const char *fmt, ...)                                 \
	{                                                                   \
		pthread_mutex_lock(&print_fast_mu);                         \
                                                                            \
		int charoffset = 0;                                         \
		va_list ap;                                                 \
		char buf[32];                                               \
		char vbuf[2048];                                            \
                                                                            \
		memset(vbuf, 0, sizeof(vbuf));                              \
		__get_time_str(buf, sizeof(buf) / sizeof(char));            \
                                                                            \
		charoffset = charoffset + snprintf(&vbuf[charoffset],       \
						   sizeof(vbuf) - 4, " %s", \
						   buf);                    \
                                                                            \
		charoffset = charoffset + snprintf(&vbuf[charoffset],       \
						   sizeof(vbuf) - 4,        \
						   LOG_PREFIX);             \
                                                                            \
		charoffset = charoffset + vsnprintf(&vbuf[charoffset],      \
						    sizeof(vbuf) - 4, fmt,  \
						    ap);                    \
                                                                            \
		fwrite(vbuf, sizeof(char), charoffset, stdout);             \
                                                                            \
		va_end(ap);                                                 \
		pthread_mutex_unlock(&print_fast_mu);                         \
	}

int __get_time_str(char *buf, size_t n);
DEFINE_HDR_PR_FUNCTION(pr_emerg);
DEFINE_HDR_PR_FUNCTION(pr_alert);
DEFINE_HDR_PR_FUNCTION(pr_crit);
DEFINE_HDR_PR_FUNCTION(pr_err);
DEFINE_HDR_PR_FUNCTION(pr_warning);
DEFINE_HDR_PR_FUNCTION(pr_notice);
DEFINE_HDR_PR_FUNCTION(pr_info);
DEFINE_HDR_PR_FUNCTION(pr_debug);
DEFINE_HDR_PR_FUNCTION(pr_cont);

#endif /* _PRINT_H_ */