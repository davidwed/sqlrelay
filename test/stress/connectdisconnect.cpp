// Copyright (c) 2016  David Muse
// See the file COPYING for more information.

#include <rudiments/inetsocketclient.h>
#include <rudiments/snooze.h>

int main() {

	inetsocketclient	isc;

	uint64_t	i=0;
	for (;;) {
		if (!isc.connect("sqlrserver",9000,-1,-1,0,1)) {
			stdoutput.printf("%d: connect/disconnect success\n",i);
		} else {
			stdoutput.printf("%d: connect/disconnect failed\n",i);
		}
		isc.close();
		i++;
	}
}
