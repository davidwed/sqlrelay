// Copyright (c) 2012 David Muse
// See the COPYING file for more information.

#ifndef SQLRSERVER_DLL_H
#define SQLRSERVER_DLL_H

#ifdef _WIN32
	#ifdef LIBSQLRSERVER_EXPORTS
		#define SQLRSERVER_DLLSPEC __declspec(dllexport)
	#else
		#define SQLRSERVER_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define SQLRSERVER_DLLSPEC
#endif

#endif
