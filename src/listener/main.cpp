// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>
#include <rudiments/process.h>

sqlrlistener	*lsnr;

RETSIGTYPE	crash(int signum) {
	delete lsnr;
	process::exit(0);
}

RETSIGTYPE	shutDown(int signum) {
	delete lsnr;
	process::exit(0);
}

int	main(int argc, const char **argv) {

	#include <version.h>

	// launch the listener
	lsnr=new sqlrlistener();
	lsnr->handleShutDown(shutDown);
	lsnr->handleCrash(crash);
	if (lsnr->initListener(argc,argv)) {
		lsnr->listen();
	}

	// unsuccessful completion
	delete lsnr;
	process::exit(1);
}
