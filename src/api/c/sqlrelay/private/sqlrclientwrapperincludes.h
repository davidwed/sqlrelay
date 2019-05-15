// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <rudiments/private/inttypes.h>

#ifndef SQLRCLIENT_DLLSPEC
	#ifdef _WIN32
		#ifdef SQLRCLIENT_EXPORTS
			#define SQLRCLIENT_DLLSPEC __declspec(dllexport)
		#else
			#define SQLRCLIENT_DLLSPEC __declspec(dllimport)
		#endif
	#else
		#define SQLRCLIENT_DLLSPEC
	#endif
#endif
