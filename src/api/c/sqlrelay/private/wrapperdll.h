// Copyright (c) 2012 David Muse
// See the COPYING file for more information.

#ifndef SQLRCLIENTWRAPPER_DLL_H
#define SQLRCLIENTWRAPPER_DLL_H

#ifdef _WIN32
	#ifdef LIBSQLRCLIENTWRAPPER_EXPORTS
		#define SQLRCLIENTWRAPPER_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRCLIENTWRAPPER_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRCLIENTWRAPPER_DLLSPEC
#endif

#endif
