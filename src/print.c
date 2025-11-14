#include "print.h"

#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int __get_time_str(char *buf, size_t n)
{
	time_t rawtime;
	struct tm *tm;

	time(&rawtime);
	tm = localtime(&rawtime);

	return snprintf(buf, n, "[%d:%02d:%02d]", tm->tm_hour, tm->tm_min,
			tm->tm_sec);
}

DEFINE_PR_FUNCTION(pr_emerg, " emerg: ");
DEFINE_PR_FUNCTION(pr_alert, " alert: ");
DEFINE_PR_FUNCTION(pr_crit, " crit: ");
DEFINE_PR_FUNCTION(pr_err, " err: ");
DEFINE_PR_FUNCTION(pr_warning, " warning: ");
DEFINE_PR_FUNCTION(pr_notice, " notice: ");
DEFINE_PR_FUNCTION(pr_info, " info: ");
DEFINE_PR_FUNCTION(pr_debug, " debug: ");
DEFINE_PR_FUNCTION(pr_cont, " cont: ");