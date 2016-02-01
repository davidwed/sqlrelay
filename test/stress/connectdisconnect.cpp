// Copyright (c) 2016  David Muse
// See the file COPYING for more information.

#include <rudiments/commandline.h>
#include <rudiments/inetsocketclient.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>

int main(int argc, const char **argv) {

	commandline	cmdl(argc,argv);

	if (!cmdl.found("host") || !cmdl.found("port")) {
		stdoutput.printf("usage: connectdisconnect -host host -port port\n");
		process::exit(1);
	}

	const char	*host=cmdl.getValue("host");
	uint16_t	port=charstring::toUnsignedInteger(
						cmdl.getValue("port"));

	inetsocketclient	isc;

	uint64_t	i=0;
	for (;;) {
		if (isc.connect(host,port,-1,-1,0,1)) {
			stdoutput.printf("%d: connect/disconnect success\n",i);
		} else {
			stdoutput.printf("%d: connect/disconnect failed\n",i);
		}
		isc.close();
		i++;
	}
}
