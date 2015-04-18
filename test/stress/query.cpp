#include <sqlrelay/sqlrclient.h>
#include <rudiments/randomnumber.h>
#include <rudiments/datetime.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

const char	*host;
uint16_t	port;
const  char	*sock;
const char	*login;
const char	*password;
const char	*query;

int main(int argc, char **argv) {

	if (argc<2) {
		stdoutput.printf("usage: query \"query\"\n");
		process::exit(1);
	}

	//host="sqlrserver";
	host="192.168.123.13";
	port=9000;
	sock="/tmp/test.socket";
	login="test";
	password="test";
	query=argv[1];

	datetime	dt;
	dt.getSystemDateAndTime();

	uint32_t	seed=dt.getEpoch();
	for (;;) {

		sqlrconnection	sqlrcon(host,port,sock,login,password,0,1);
		sqlrcursor	sqlrcur(&sqlrcon);

		seed=randomnumber::generateNumber(seed);
		int32_t	count=randomnumber::scaleNumber(seed,1,50);

		stdoutput.printf("looping %d times\n",count);
		int32_t	successcount=0;
		for (int32_t i=0; i<count; i++) {
			if (!sqlrcur.sendQuery(query)) {
				stdoutput.printf("error: %s\n",
						sqlrcur.errorMessage());
				break;
			} else {
				successcount++;
			}
		}
		stdoutput.printf("%d succeeded\n",successcount);
	}
}
