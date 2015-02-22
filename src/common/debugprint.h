// Copyright (c) 1999-2014  David Muse
// See the file COPYING for more information

#ifndef SQLDEBUGPRINT_H
#define SQLDEBUGPRINT_H

//#define DEBUG_MESSAGES 1
//#define DEBUG_TO_FILE 1

#ifdef DEBUG_TO_FILE
	#include <rudiments/file.h>
	#include <rudiments/permissions.h>
# else
	#include <rudiments/stdio.h>
#endif

#ifdef DEBUG_MESSAGES
	#ifdef DEBUG_TO_FILE
		#define debugFunction() { file f; f.open("/tmp/sqlrdebug.txt",O_RDWR|O_APPEND|O_CREAT,permissions::evalPermString("rw-r--r--")); f.printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); f.close(); }
		#ifdef _MSC_VER
			#define debugPrintf(args,...) { file f; f.open("/tmp/sqlrdebug.txt",O_RDWR|O_APPEND|O_CREAT,permissions::evalPermString("rw-r--r--")); f.printf(args,__VA_ARGS__); f.close(); }
		#else
			#define debugPrintf(args...) { file f; f.open("/tmp/sqlrdebug.txt",O_RDWR|O_APPEND|O_CREAT,permissions::evalPermString("rw-r--r--")); f.printf(args); f.close(); }
		#endif
	#else
		#define debugFunction() stdoutput.printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); stdoutput.flush();
		#ifdef _MSC_VER
			#define debugPrintf(args,...) stdoutput.printf(args,__VA_ARGS__); stdoutput.flush();
		#else
			#define debugPrintf(args...) stdoutput.printf(args); stdoutput.flush();
		#endif
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
