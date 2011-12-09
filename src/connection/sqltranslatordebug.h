#ifndef SQLTRANSLATOR_DEBUG_H
#define SQLTRANSLATOR_DEBUG_H

#include <stdio.h>

//#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); fflush(stdout);
	#define debugPrintf(args...) printf(args); fflush(stdout);
#else
	#define debugFunction() /* */
	#define debugPrintf(args...) /* */
#endif

#endif
