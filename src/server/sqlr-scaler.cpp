// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrscaler.h>
#include <rudiments/process.h>
#include <config.h>
#include <version.h>

static void helpmessage() {
	stdoutput.printf(
		"%s-scaler is the SQL Relay database connection scaling daemon.\n"
		"\n"
		"The %s-scaler monitors connections from SQL Relay client applications and opens new database connections, as-necessary.\n"
		"\n"
		"The %s-scaler is not intended to be run manually.  Rather the %s-start process spawns it as-necessary.\n"
		"\n"
		"Usage: %s-scaler [OPTIONS]\n"
		"\n"
		"Options:\n"
		SERVEROPTIONS
		DISABLECRASHHANDLER
		REPORTBUGS,
		SQLR,SQLR,SQLR,SQLR,SQLR);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	{
		scaler	s;
		if (s.initScaler(argc,argv)) {
			s.loop();
		}
	}
	process::exit(1);
}
