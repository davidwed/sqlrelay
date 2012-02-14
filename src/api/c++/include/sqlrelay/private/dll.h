// Copyright (c) 2012 David Muse
// See the COPYING file for more information.

#ifndef SQLRCLIENT_DLL_H
#define SQLRCLIENT_DLL_H

#ifdef _WIN32
	#ifdef LIBSQLRCLIENT_EXPORTS
		#define SQLRCLIENT_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRCLIENT_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRCLIENT_DLLSPEC
#endif

#endif
