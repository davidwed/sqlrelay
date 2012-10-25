// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <scaler.h>
#include <rudiments/process.h>
#include <rudiments/charstring.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

scaler	*s;

void shutDown(int32_t signum) {
	s->shutDown();
}

int main(int argc, const char **argv) {

	#include <version.h>

	s=new scaler();
	s->handleShutDown(shutDown);
	s->handleCrash(shutDown);
	if (s->initScaler(argc,argv)) {
		s->loop();
	}
	delete s;
	process::exit(1);
}
