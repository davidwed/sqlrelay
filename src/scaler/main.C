// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <scaler.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

scaler	*s;

void cleanUp() {
	delete s;
}

RETSIGTYPE shutDown() {
	cleanUp();
	exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	// launch the scaler
	s=new scaler();
	s->handleShutDown((RETSIGTYPE *)shutDown);
	if (s->initScaler(argc,argv)) {
		s->loop();
	}

	// unsuccessful completion
	cleanUp();
	exit(1);
}
