// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>

sqlrlistener	*lsnr;

RETSIGTYPE	crash() {
	delete lsnr;
	exit(0);
}

RETSIGTYPE	shutDown() {
	delete lsnr;
	exit(0);
}

int	main(int argc, const char **argv) {

	#include <version.h>

	// launch the listener
	lsnr=new sqlrlistener();
	lsnr->handleShutDown((RETSIGTYPE *)shutDown);
	lsnr->handleCrash((RETSIGTYPE *)crash);
	if (lsnr->initListener(argc,argv)) {
		lsnr->listen();
	}

	// unsuccessful completion
printf("unsuccessful completion\n");
	delete lsnr;
	exit(1);
}
