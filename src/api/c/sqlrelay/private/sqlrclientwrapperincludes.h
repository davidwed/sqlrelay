// Copyright (c) 2016 David Muse
// See the COPYING file for more information.

#include <rudiments/private/inttypes.h>

#ifdef _WIN32
	#ifdef SQLRCLIENTWRAPPER_EXPORTS
		#define SQLRCLIENTWRAPPER_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRCLIENTWRAPPER_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRCLIENTWRAPPER_DLLSPEC
#endif
