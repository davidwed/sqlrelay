// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrlistener.h>
#include <rudiments/process.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

sqlrlistener	*lsnr;

void shutDown(int32_t signum) {
	delete lsnr;
	process::exit(0);
}

int	main(int argc, const char **argv) {

	#include <version.h>

	lsnr=new sqlrlistener();
	lsnr->handleShutDown(shutDown);
	lsnr->handleCrash(shutDown);
	if (lsnr->initListener(argc,argv)) {
		lsnr->listen();
	}
	delete lsnr;
	process::exit(1);
}
