// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/charstring.h>
#include <rudiments/stdio.h>
#include <rudiments/process.h>
#include <rudiments/snooze.h>

const char	*host;
uint16_t	port;
const char	*sock;
const char	*user;
const char	*password;
int64_t		concount;
bool		terminated;

void shutDown(int32_t signum) {
	terminated=true;
}

int main(int argc, const char **argv) {

	terminated=false;

	process::setShutDownHandler(shutDown);

	commandline	cmdl(argc,argv);

	if (!cmdl.isFound("concount")) {
		stdoutput.printf("usage: churn -host host -port port -socket socket -user user -password password -concount concount\n");
		process::exit(1);
	}

	host=cmdl.getValue("host");
	port=charstring::convertToUnsignedInteger(cmdl.getValue("port"));
	sock=cmdl.getValue("socket");
	user=cmdl.getValue("user");
	password=cmdl.getValue("password");
	concount=charstring::convertToUnsignedInteger(cmdl.getValue("concount"));

	sqlrconnection	**cons=new sqlrconnection *[concount];

	for (int64_t i=0; i<concount; i++) {
		cons[i]=new sqlrconnection(host,port,sock,user,password,0,1);
	}

	while (!terminated) {

		stdoutput.printf("pinging %lld connections...\n",concount);
		bool	success=true;
		for (int64_t i=0; i<concount && !terminated; i++) {
			if (!cons[i]->ping()) {
				success=false;
				stdoutput.printf("ping to %d failed:\n%s\n",
						i,cons[i]->errorMessage());
			}
		}
		if (!success) {
			// loop forever to leave things
			// as they are for investigation
			stdoutput.printf("leaving things as they are...\n");
			while (!terminated) {
				snooze::macrosnooze(1);
			}
			break;
		}

		/*if (!terminated) {
			stdoutput.printf("sleeping 3 seconds...\n");
			snooze::macrosnooze(3);
		}*/

		stdoutput.printf("closing %lld connections...\n",concount);
		for (int64_t i=0; i<concount && !terminated; i++) {
			cons[i]->endSession();
		}
	}

	for (int64_t i=0; i<concount && !terminated; i++) {
		delete cons[i];
	}
	delete[] cons;

	process::exit(1);
}
