// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

sqlrlistener	*lsnr;

RETSIGTYPE	shutDown() {
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
	delete lsnr;
	exit(1);
}
