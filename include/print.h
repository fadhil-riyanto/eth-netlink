#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdio.h>
#include <sys/types.h>
#if defined(__linux__) && defined(__unix__)
	#include <pthread.h>
	static pthread_mutex_t print_fast_mu = PTHREAD_MUTEX_INITIALIZER;
#endif

#define DEFINE_PR_FUNCTION(NAME, LOG_PREFIX) \
	void __##NAME()                      \
	{                                    \
		printf("%s\n", LOG_PREFIX);  \
	}
	
#define DEFINE_HDR_PR_FUNCTION(NAME, LOG_PREFIX) \
	void __##NAME();

DEFINE_HDR_PR_FUNCTION(pr_err, "error: ");

#endif /* _PRINT_H_ */