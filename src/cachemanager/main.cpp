// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <cachemanager.h>

#include <rudiments/charstring.h>
#include <rudiments/process.h>

// for printf() in version.h
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

cachemanager	*cacheman;

void shutDown(int32_t signum) {
	delete cacheman;
	process::exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	cacheman=new cachemanager(argc,argv);
	cacheman->handleShutDown(shutDown);
	cacheman->scan();
}
