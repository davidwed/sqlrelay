// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrscaler.h>
#include <rudiments/process.h>
#include <config.h>
#include <version.h>

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s database connection scaling daemon.\n"
		"\n"
		"The %s monitors connections from %s client applications and opens new database connections, as-necessary.\n"
		"\n"
		"The %s is not intended to be run manually.  Rather the %s-start process spawns it as-necessary.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		DISABLECRASHHANDLER,
		progname,SQL_RELAY,progname,SQL_RELAY,progname,SQLR,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	commandline	cmdl(argc,argv);

	if (!cmdl.found("-id")) {
		stdoutput.printf("usage: \n"
			" %s-scaler [-config config] -id id "
			"[-localstatedir dir]\n",
			SQLR);
		process::exit(0);
	}

	{
		scaler	s;
		if (s.initScaler(argc,argv)) {
			s.loop();
		}
	}
	process::exit(1);
}
