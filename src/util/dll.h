// Copyright (c) 2012 David Muse
// See the COPYING file for more information.

#ifndef SQLRUTIL_DLL_H
#define SQLRUTIL_DLL_H

#ifdef _WIN32
	#ifdef LIBSQLRUTIL_EXPORTS
		#define SQLRUTIL_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRUTIL_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRUTIL_DLLSPEC
#endif

#endif
