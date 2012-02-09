// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <scaler.h>
#include <rudiments/process.h>

scaler	*s;

void cleanUp() {
	delete s;
}

RETSIGTYPE shutDown(int signum) {
	cleanUp();
	process::exit(0);
}

int main(int argc, const char **argv) {

	#include <version.h>

	// launch the scaler
	s=new scaler();
	s->handleShutDown(shutDown);
	if (s->initScaler(argc,argv)) {
		s->loop();
	}

	// unsuccessful completion
	cleanUp();
	process::exit(1);
}
