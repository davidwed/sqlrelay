// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <cachemanager.h>

#include <rudiments/charstring.h>

// for exit()
#include <stdlib.h>

// for printf() in version.h
#include <stdio.h>

cachemanager	*cacheman;

void shutDown(int signum) {
	delete cacheman;
	exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	cacheman=new cachemanager(argc,argv);
	cacheman->handleShutDown(shutDown);
	cacheman->scan();
}
