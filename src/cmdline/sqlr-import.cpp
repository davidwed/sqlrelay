// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <sqlrelay/sqlrimportxml.h>
#include <sqlrelay/sqlrimportcsv.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s database object import utility.\n"
		"\n"
		"Import a database object from a file created previously or elsewhere using\n"
		"%s-export.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		"\n"
		CONNECTIONOPTIONS
		"\n"
		"Command options:\n"
		"	-file filename		Import the specified file.\n"
		"\n"
		"	-commitcount rowcount	Execute a commit each time the specified number\n"
		"				of rows have been imported since the beginning\n"
		"				or since the previous commit.\n"
		"\n"
		"	-debug [filename]	Write debug information to the specified file\n"
		"				or to the screen if no file is specified.\n"
		"\n"
		"	-verbose		Display details about the import process.\n"
		"\n"
		"Examples:\n"
		"\n"
		"Import a table and sequence using the server at svr:9000 as usr/pwd.\n"
		"\n"
		"	%s -host svr -port 9000 -user usr -password pwd \\\n"
		"		-file mytable.tbl\n"
		"\n"
		"	%s -host svr -port 9000 -user usr -password pwd \\\n"
		"		-file myseq.seq\n"
		"\n"
		"Import a table and sequence using the local server on socket /tmp/svr.sock\n"
		"as usr/pwd.\n"
		"\n"
		"	%s -socket /tmp/svr.sock -user usr -password pwd \\\n"
		"		-file mytable.tbl\n"
		"\n"
		"	%s -socket /tmp/svr.sock -user usr -password pwd \\\n"
		"		-file myseq.seq\n"
		"\n"
		"Import a table and sequence using connection info and credentials from\n"
		"an instance defined in the default configuration.\n"
		"\n"
		"	%s -id myinst -file mytable.tbl\n"
		"\n"
		"	%s -id myinst -file myseq.seq\n"
		"\n"
		"Import a table and sequence using connection info and credentials from\n"
		"in instance defined in the config file ./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst -file mytable.tbl\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst -file myseq.seq\n"
		"\n",
		progname,SQL_RELAY,SQLR,progname,
		progname,progname,progname,progname,
		progname,progname,progname,progname);
}

int main(int argc, const char **argv) {

	version(argc,argv);
	help(argc,argv);

	sqlrcmdline 	cmdline(argc,argv);
	sqlrpaths	sqlrpth(&cmdline);
	sqlrconfigs	sqlrcfgs(&sqlrpth);

	// get command-line options
	const char	*configurl=sqlrpth.getConfigUrl();
	const char	*id=cmdline.getValue("id");
	const char	*host=cmdline.getValue("host");
	uint16_t	port=charstring::toInteger(
				(cmdline.found("port"))?
				cmdline.getValue("port"):DEFAULT_PORT);
	const char	*socket=cmdline.getValue("socket");
	const char	*user=cmdline.getValue("user");
	const char	*password=cmdline.getValue("password");
	bool		usekrb=cmdline.found("krb");
	const char	*krbservice=cmdline.getValue("krbservice");
	const char	*krbmech=cmdline.getValue("krbmech");
	const char	*krbflags=cmdline.getValue("krbflags");
	bool		usetls=cmdline.found("tls");
	const char	*tlscert=cmdline.getValue("tlscert");
	const char	*tlsversion=cmdline.getValue("tlsversion");
	const char	*tlspassword=cmdline.getValue("tlspassword");
	const char	*tlsciphers=cmdline.getValue("tlsciphers");
	const char	*tlsvalidate="no";
	if (cmdline.found("tlsvalidate")) {
		tlsvalidate=cmdline.getValue("tlsvalidate");
	}
	const char	*tlsca=cmdline.getValue("tlsca");
	uint16_t	tlsdepth=charstring::toUnsignedInteger(
					cmdline.getValue("tlsdepth"));
	const char	*file=cmdline.getValue("file");
	const char	*commitcountstr=cmdline.getValue("commitcount");
	bool		debug=cmdline.found("debug");
	const char	*debugfile=NULL;
	if (debug) {
		debugfile=cmdline.getValue("debug");
	}
	bool		verbose=cmdline.found("verbose");
	const char	*table=cmdline.getValue("table");
	bool 		ignorecolumns=cmdline.found("ignorecolumns");

	// at least id, host or socket, and file are required
	if ((charstring::isNullOrEmpty(id) &&
		charstring::isNullOrEmpty(host) &&
		charstring::isNullOrEmpty(socket)) ||
		charstring::isNullOrEmpty(file)) {

		stdoutput.printf("usage: \n"
			" %s-import -host host -port port -socket socket\n"
			"        [-user user -password password]\n"
			"        [-krb] [-krbservice svc] [-krbmech mech] "
			"[-krbflags flags]\n"
			"        [-tls] [-tlsversion version]\n"
			"        [-tlscert certfile] [-tlspassword password]\n"
			"        [-tlsciphers cipherlist]\n"
			"        [-tlsvalidate (yes|no)] [-tlsca ca] "
			"[-tlsdepth depth]\n"
			"        -file file [-commitcount rowcount]\n"
			"        [-debug [filename]] [-verbose]\n"
			"  or\n"
			" %s-import [-config config] -id id\n"
			"        -file file [-commitcount rowcount]\n"
			"        [-debug [filename]] [-verbose]\n",
			SQLR,SQLR);
		process::exit(1);
	}

	// if an id was specified, then get various values from the config file
	if (!charstring::isNullOrEmpty(id)) {
		sqlrconfig	*cfg=sqlrcfgs.load(configurl,id);
		if (cfg) {
			if (!cmdline.found("host")) {
				host="localhost";
			}
			if (!cmdline.found("port")) {
				port=cfg->getDefaultPort();
			}
			if (!cmdline.found("socket")) {
				socket=cfg->getDefaultSocket();
			}
			if (!cmdline.found("krb")) {
				usekrb=cfg->getDefaultKrb();
			}
			if (!cmdline.found("krbservice")) {
				krbservice=cfg->getDefaultKrbService();
			}
			if (!cmdline.found("krbmech")) {
				krbmech=cfg->getDefaultKrbMech();
			}
			if (!cmdline.found("krbflags")) {
				krbflags=cfg->getDefaultKrbFlags();
			}
			if (!cmdline.found("tls")) {
				usetls=cfg->getDefaultTls();
			}
			if (!cmdline.getValue("tlsciphers")) {
				tlsciphers=cfg->getDefaultTlsCiphers();
			}
			if (!cmdline.found("user")) {
				user=cfg->getDefaultUser();
				password=cfg->getDefaultPassword();
			}
		}
	}

	// configure sql relay connection
	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	// configure kerberos
	if (usekrb) {
		sqlrcon.enableKerberos(krbservice,krbmech,krbflags);
	} else if (usetls) {
		sqlrcon.enableTls(tlsversion,tlscert,tlspassword,tlsciphers,
						tlsvalidate,tlsca,tlsdepth);
	}

	// configure debug
	if (debug) {
		if (debugfile) {
			sqlrcon.setDebugFile(debugfile);
		}
		sqlrcon.debugOn();
	}

	// set commitcount
	// (if not specified at all, default to 100)
	uint64_t	commitcount=
			(charstring::isNullOrEmpty(commitcountstr))?
				100:charstring::toInteger(commitcountstr);

	// set up logging
	logger	lg;
	stdoutdestination	std;
	lg.addLogDestination(&std);

	// xml or csv
	sqlrimport	*sqlri;
	if (!charstring::compareIgnoringCase(
			charstring::findLast(file,'.'),".csv")) {
		sqlri=new sqlrimportcsv();
	} else {
		sqlri=new sqlrimportxml();
	}
	sqlri->setSqlrConnection(&sqlrcon);
	sqlri->setSqlrCursor(&sqlrcur);
	sqlri->setDbType(sqlrcon.identify());
	if (!charstring::isNullOrEmpty(table)) {
		sqlri->setTable(table);
	}
	sqlri->setIgnoreColumns(ignorecolumns);
	sqlri->setCommitCount(commitcount);
	if (verbose) {
		sqlri->setLogger(&lg);
		sqlri->setCoarseLogLevel(1);
		sqlri->setFineLogLevel(1);
		sqlri->setLogIndent(1);
	}
	bool	success=sqlri->importFromFile(file);
	delete sqlri;
	process::exit(!success);
}
