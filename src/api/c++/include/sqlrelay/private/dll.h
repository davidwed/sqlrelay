// Copyright (c) 2012 David Muse
// See the COPYING file for more information.

#ifndef SQLRCLIENT_DLL_H
#define SQLRCLIENT_DLL_H

#ifdef _WIN32
	#ifdef LIBSQLRCLIENT_EXPORTS
		#define DLLSPEC __declspec(dllexport)
	#else
		#define DLLSPEC __declspec(dllimport)
	#endif
#else
	#define DLLSPEC
#endif

#endif
