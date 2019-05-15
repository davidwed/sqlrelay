// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <rudiments/stringbuffer.h>
#include <rudiments/parameterstring.h>
#include <rudiments/regularexpression.h>
#include <rudiments/commandline.h>
#include <rudiments/linkedlist.h>
#include <rudiments/domnode.h>
#include <rudiments/dynamiclib.h>

#ifndef SQLRUTIL_DLLSPEC
	#ifdef _WIN32
		#ifdef SQLRUTIL_EXPORTS
			#define SQLRUTIL_DLLSPEC __declspec(dllexport)
		#else
			#define SQLRUTIL_DLLSPEC __declspec(dllimport)
		#endif
	#else
		#define SQLRUTIL_DLLSPEC
	#endif
#endif
