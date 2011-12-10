// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLTRANSLATOR_DEBUG_H
#define SQLTRANSLATOR_DEBUG_H

#include <stdio.h>

//#define DEBUG_MESSAGES 1
#ifdef DEBUG_MESSAGES
	#define debugFunction() printf("%s:%s():%d:\n",__FILE__,__FUNCTION__,__LINE__); fflush(stdout);
	#define debugPrintf(args...) printf(args); fflush(stdout);
	#define debugPrintQueryTree(tree) { \
		const char	*xml=tree->getRootNode()->xml()->getString(); \
		int16_t		indent=0; \
		bool		endtag; \
		for (const char *ptr=xml; *ptr; ptr++) { \
			if (*ptr=='<') { \
				if (*(ptr+1)=='/') { \
					indent=indent-2; \
					endtag=true; \
				} \
				for (uint16_t i=0; i<indent; i++) { \
					printf(" "); \
				} \
			} \
			printf("%c",*ptr); \
			if (*ptr=='>') { \
				printf("\n"); \
				if (*(ptr-1)!='/' && !endtag) { \
					indent=indent+2; \
				} \
				endtag=false; \
			} \
		} \
	}
#else
	#define debugFunction() /* */
	#define debugPrintf(args...) /* */
	#define debugPrintQueryTree(tree) /* */
#endif

#endif
