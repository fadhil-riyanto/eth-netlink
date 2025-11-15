#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>

// #define SEND_TO_SYSLOG

#define __setup_syslog() \
	openlog("ethnetlink", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

#define __close_syslog() \
	closelog();

#ifdef SEND_TO_SYSLOG
# include <syslog.h>
#  define SYSLOG_CTOR								\
	__setup_syslog()

#  define SEND_SYSLOG(BUF)							\
	syslog(LOG_NOTICE, BUF);						\

#  define SYSLOG_DTOR								\
	__close_syslog()
#else
# define __NOP __asm__ volatile ("nop")
#  define SYSLOG_CTOR __NOP;
#  define SEND_SYSLOG(BUF) __NOP;
#  define SYSLOG_DTOR __NOP;

#endif /* SEND_TO_SYSLOG */


#if defined(__linux__) && defined(__unix__)
#include <pthread.h>
	static pthread_mutex_t print_fast_mu = PTHREAD_MUTEX_INITIALIZER;
#  ifdef SEND_TO_SYSLOG
	
#  endif /* SEND_TO_SYSLOG */
#endif

#define DEFINE_HDR_PR_FUNCTION(NAME) 			\
	void __##NAME(const char *fmt, ...);

#define DEFINE_PR_FUNCTION(NAME, LOG_PREFIX)                                \
	void __##NAME(const char *fmt, ...)                                 \
	{                                                                   \
		pthread_mutex_lock(&print_fast_mu);                         \
                                                                            \
		SYSLOG_CTOR						    \
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
		SEND_SYSLOG(vbuf)					    \
                                                                            \
		va_end(ap);                                                 \
                                                                            \
		SYSLOG_DTOR						    \
									    \
		pthread_mutex_unlock(&print_fast_mu);                       \
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

#define pr_emerg __pr_emerg
#define pr_alert __pr_alert
#define pr_crit __pr_crit
#define pr_err __pr_err
#define pr_warning __pr_warning
#define pr_notice __pr_notice
#define pr_info __pr_info
#define pr_debug __pr_debug
#define pr_cont __pr_cont

#endif /* _PRINT_H_ */