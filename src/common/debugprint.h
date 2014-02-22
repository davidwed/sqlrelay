// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_DEBUG_H
#define SQLTRANSLATOR_DEBUG_H

#include <rudiments/stdio.h>
#include <stdio.h>

//#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() stdoutput.printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); fflush(stdout);
	#ifdef _MSC_VER
		#define debugPrintf(args,...) stdoutput.printf(args,__VA_ARGS__); fflush(stdout);
	#else
		#define debugPrintf(args...) stdoutput.printf(args); fflush(stdout);
	#endif
#else
	#define debugFunction() /* */
	#ifdef _MSC_VER
		#define debugPrintf(args,...) /* */
	#else
		#define debugPrintf(args...) /* */
	#endif
#endif

#endif
