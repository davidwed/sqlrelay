// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <sqlrconfigfile.h>

int main(int argc, const char **argv) {

	#include <version.h>

	sqlrconfigfile	cfgfile;

	commandline	cmdline(argc,argv);

	const char	*config=cmdline.getValue("-config");
	const char	*id=cmdline.getValue("-id");
	const char	*host=cmdline.getValue("-host");
	uint16_t	port=charstring::toInteger(
				(cmdline.found("-port"))?
					cmdline.getValue("-port"):DEFAULT_PORT);
	const char	*socket=cmdline.getValue("-socket");
	const char	*user=cmdline.getValue("-user");
	const char	*password=cmdline.getValue("-password");
	const char	*query=cmdline.getValue("-query");
	uint64_t	rsbs=charstring::toInteger(
				cmdline.getValue("-resultsetbuffersize"));
	if (!rsbs) {
		rsbs=100;
	}
	bool		debug=cmdline.found("-debug");
	const char	*debugfile=NULL;
	if (debug) {
		debugfile=cmdline.getValue("-debug");
	}

	if (!(charstring::length(id) ||
		charstring::length(host) ||
		charstring::length(socket)) ||
		!(charstring::length(query))) {

		stdoutput.printf("usage: sqlr-query -host host -port port -socket socket -user user -password password -query query [-debug [filename]] [-resultsetbuffersize rows]\n"
			"  or   sqlr-query  [-config configfile] -id id -query query [-debug [filename]] [-resultsetbuffersize rows]\n");
		process::exit(1);
	}

	if (charstring::length(id) && cfgfile.parse(config,id)) {

		// get the host/port/socket/username/password
		host="localhost";
		port=cfgfile.getPort();
		socket=cfgfile.getUnixPort();
		usercontainer	*currentnode=
				cfgfile.getUserList()->getFirst()->getValue();
		user=currentnode->getUser();
		password=currentnode->getPassword();
	}

	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	if (debug) {
		if (debugfile) {
			sqlrcon.setDebugFile(debugfile);
		}
		sqlrcon.debugOn();
	}

	int32_t	exitval=0;
	sqlrcur.dontGetColumnInfo();
	sqlrcur.setResultSetBufferSize(rsbs);
	if (sqlrcur.sendQuery(query)) {
		uint64_t	i=0;
		uint32_t	cols=sqlrcur.colCount();
		const char	*field="";
		while (cols && field) {
			for (uint32_t j=0; j<cols; j++) {
				if ((field=sqlrcur.getField(i,j))) {
					stdoutput.printf("\"%s\"",field);
					if (j<cols-1) {
						stdoutput.printf(",");
					} else {
						stdoutput.printf("\n");
					}
				}
			}
			i++;
		}
	} else {
		stdoutput.printf("%s\n",sqlrcur.errorMessage());
		exitval=1;
	}
	sqlrcon.endSession();

	process::exit(exitval);
}
