// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#ifndef SQLDEBUGPRINT_H
#define SQLDEBUGPRINT_H

#include <rudiments/stdio.h>

//#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() stdoutput.printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); stdoutput.flush();
	#ifdef _MSC_VER
		#define debugPrintf(args,...) stdoutput.printf(args,__VA_ARGS__); stdoutput.flush();
	#else
		#define debugPrintf(args...) stdoutput.printf(args); stdoutput.flush();
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
