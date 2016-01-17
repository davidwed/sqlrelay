// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrscaler.h>
#include <rudiments/process.h>
#include <config.h>
#include <version.h>

static void helpmessage() {
	stdoutput.printf("FIXME: implement this\n");
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
