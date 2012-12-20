// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrscaler.h>

#include <rudiments/process.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int main(int argc, const char **argv) {

	#include <version.h>
	{
		scaler	s;
		if (s.initScaler(argc,argv)) {
			s.loop();
		}
	}
	process::exit(1);
}
