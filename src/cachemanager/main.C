// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <cachemanager.h>
#include <stdio.h>
#include <string.h>

cachemanager	*cacheman;

void shutDown() {
	delete cacheman;
	exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	cacheman=new cachemanager(argc,argv);
	cacheman->handleShutDown((void *)shutDown);
	cacheman->scan();
}
