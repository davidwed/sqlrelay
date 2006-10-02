// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>

sqlrlistener	*lsnr;

RETSIGTYPE	shutDown() {
printf("shutting down!\n");
	delete lsnr;
	exit(0);
}

int	main(int argc, const char **argv) {

	#include <version.h>

	// launch the listener
	lsnr=new sqlrlistener();
	lsnr->handleShutDown((RETSIGTYPE *)shutDown);
	if (lsnr->initListener(argc,argv)) {
		lsnr->listen();
	}

	// unsuccessful completion
printf("unsuccessful completion\n");
	delete lsnr;
	exit(1);
}
