// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#include <version.h>

static void xmlEscapeField(const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		if (field[index]=='\'') {
			stdoutput.printf("''");
		} else if (field[index]<' ' || field[index]>'~' ||
				field[index]=='&' || field[index]=='<' ||
				field[index]=='>') {
			stdoutput.printf("&%d;",(uint8_t)field[index]);
		} else {
			stdoutput.printf("%c",field[index]);
		}
	}
}

static void csvEscapeField(const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		// escape double quotes and ignore non-ascii characters
		if (field[index]=='"') {
			stdoutput.printf("\"\"");
		} else if (field[index]>=' ' || field[index]<='~') {
			stdoutput.printf("%c",field[index]);
		}
	}
}

static void exportTableXml(sqlrcursor *sqlrcur, const char *table) {

	// print header
	stdoutput.printf("<?xml version=\"1.0\"?>\n");

	// print table name
	stdoutput.printf("<table name=\"%s\">\n",table);

	// print columns
	uint32_t	cols=sqlrcur->colCount();
	stdoutput.printf("<columns count=\"%d\">\n",cols);
	for (uint32_t j=0; j<cols; j++) {
		stdoutput.printf("	<column name=\"%s\" type=\"%s\"/>\n",
			sqlrcur->getColumnName(j),sqlrcur->getColumnType(j));
	}
	stdoutput.printf("</columns>\n");

	// print rows
	stdoutput.printf("<rows>\n");
	uint64_t	row=0;
	do {
		stdoutput.printf("	<row>\n");
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			stdoutput.printf("	<field>");
			xmlEscapeField(field,sqlrcur->getFieldLength(row,col));
			stdoutput.printf("</field>\n");
		}
		stdoutput.printf("	</row>\n");
		row++;
	} while (!sqlrcur->endOfResultSet() || row<sqlrcur->rowCount());
	stdoutput.printf("</rows>\n");
	stdoutput.printf("</table>\n");
}

static void exportTableCsv(sqlrcursor *sqlrcur) {

	// print header
	uint32_t	cols=sqlrcur->colCount();
	for (uint32_t j=0; j<cols; j++) {
		if (j) {
			stdoutput.printf(",");
		}
		stdoutput.printf("%s",sqlrcur->getColumnName(j));
	}
	stdoutput.printf("\n");

	// print rows
	uint64_t	row=0;
	for (;;) {
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			if (col) {
				stdoutput.printf(",");
			}
			stdoutput.printf("\"");
			csvEscapeField(field,sqlrcur->getFieldLength(row,col));
			stdoutput.printf("\"");
		}
		stdoutput.printf("\n");
		row++;
		if (sqlrcur->endOfResultSet() && row>=sqlrcur->rowCount()) {
			break;
		}
	}
}

static bool exportTable(sqlrcursor *sqlrcur,
				const char *table, const char *format) {

	stringbuffer	query;
	query.append("select * from ")->append(table);
	if (sqlrcur->sendQuery(query.getString())) {

		if (!charstring::compareIgnoringCase(format,"csv")) {
			exportTableCsv(sqlrcur);
		} else {
			exportTableXml(sqlrcur,table);
		}
		return true;
	}

	stdoutput.printf("%s\n",sqlrcur->errorMessage());
	return false;
}

static void exportSequenceXml(sqlrcursor *sqlrcur, const char *sequence) {

	// print header
	stdoutput.printf("<?xml version=\"1.0\"?>\n");

	// print sequence value
	stdoutput.printf("<sequence name=\"%s\" value=\"%s\"/>\n",
			sequence,sqlrcur->getField(0,(uint32_t)0));
}

static void exportSequenceCsv(sqlrcursor *sqlrcur, const char *sequence) {

	// print header
	stdoutput.printf("%s\n",sequence);

	// print sequence value
	stdoutput.printf("\"%s\"\n",sqlrcur->getField(0,(uint32_t)0));
}

static bool exportSequence(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
				const char *sequence, const char *format) {

	// the query we'll use to get the sequence depends on the database type
	const char	*dbtype=sqlrcon->identify();

	stringbuffer	query;
	if (charstring::contains(dbtype,"firebird") ||
		charstring::contains(dbtype,"interbase")) {
		query.append("select gen_id(")->append(sequence);
		query.append(",1) from rdb$database");
	} else if (charstring::contains(dbtype,"oracle")) {
		query.append("select ")->append(sequence);
		query.append(".nextval from dual");
	} else if (charstring::contains(dbtype,"postgresql")) {
		query.append("select nextval('")->append(sequence);
		query.append("')");
	} else if (charstring::contains(dbtype,"db2")) {
		query.append("values nextval for ")->append(sequence);
	} else if (charstring::contains(dbtype,"informix")) {
		query.append("select ")->append(sequence);
		query.append(".nextval from sysmaster::sysdual");
	} else {
		stdoutput.printf("%s doesn't support sequences.\n",dbtype);
		return false;
	}

	if (sqlrcur->sendQuery(query.getString())) {

		if (!charstring::compareIgnoringCase(format,"csv")) {
			exportSequenceCsv(sqlrcur,sequence);
		} else {
			exportSequenceXml(sqlrcur,sequence);
		}
		return true;
	}

	stdoutput.printf("%s\n",sqlrcur->errorMessage());
	return false;
}

static void helpmessage(const char *progname) {
	stdoutput.printf(
		"%s is the %s database object export utility.\n"
		"\n"
		"Export a database object for import later or elsewhere using %s-import.\n"
		"\n"
		"Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		"\n"
		CONNECTIONOPTIONS
		"\n"
		"Command options:\n"
		"	-table tbl		Export the specified table.\n"
		"\n"
		"	-sequence seq		Export the specified sequence.\n"
		"\n"
		"	-format xml|csv		Format the output as specified.\n"
		"				Defaults to xml.\n"
		"\n"
		"Examples:\n"
		"\n"
		"Export a table and sequence using the server at svr:9000 as usr/pwd.\n"
		"\n"
		"	%s -host svr -port 9000 -user usr -password pwd \\\n"
		"		-table mytable > mytable.tbl\n"
		"\n"
		"	%s -host svr -port 9000 -user usr -password pwd \\\n"
		"		-sequence myseq > myseq.seq\n"
		"\n"
		"Export a table and sequence using the local server on socket /tmp/svr.sock\n"
		"as usr/pwd.\n"
		"\n"
		"	%s -socket /tmp/svr.sock -user usr -password pwd \\\n"
		"		-table mytable > mytable.tbl\n"
		"\n"
		"	%s -socket /tmp/svr.sock -user usr -password pwd \\\n"
		"		-sequence myseq > myseq.seq\n"
		"\n"
		"Export a table and sequence using connection info and credentials from\n"
		"an instance defined in the default configuration.\n"
		"\n"
		"	%s -id myinst -table mytable > mytable.tbl\n"
		"\n"
		"	%s -id myinst -sequence myseq > myseq.seq\n"
		"\n"
		"Export a table and sequence using connection info and credentials from\n"
		"an instance defined in the config file ./myconfig.conf\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst \\\n"
		"		-table mytable > mytable.tbl\n"
		"\n"
		"	%s -config ./myconfig.conf -id myinst \\\n"
		"		-sequence myseq > myseq.seq\n"
		"\n",
		progname,SQL_RELAY,SQLR,progname,progname,
		progname,progname,progname,progname,progname,progname,progname);
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
	const char	*krbservice=cmdline.getValue("krb");
	const char	*krbmech=cmdline.getValue("krbmech");
	const char	*krbflags=cmdline.getValue("krbflags");
	bool		usetls=cmdline.found("tls");
	const char	*tlsversion=cmdline.getValue("tlsversion");
	const char	*tlscert=cmdline.getValue("tlscert");
	const char	*tlspassword=cmdline.getValue("tlspassword");
	const char	*tlsciphers=cmdline.getValue("tlsciphers");
	const char	*tlsvalidate="no";
	if (cmdline.found("tlsvalidate")) {
		tlsvalidate=cmdline.getValue("tlsvalidate");
	}
	const char	*tlsca=cmdline.getValue("tlsca");
	uint16_t	tlsdepth=charstring::toUnsignedInteger(
					cmdline.getValue("tlsdepth"));
	const char	*table=cmdline.getValue("table");
	const char	*sequence=cmdline.getValue("sequence");
	const char	*format=cmdline.getValue("format");
	if (charstring::isNullOrEmpty(format)) {
		format="xml";
	}
	uint64_t	rsbs=charstring::toInteger(
				cmdline.getValue("resultsetbuffersize"));
	if (!rsbs) {
		rsbs=100;
	}
	bool		debug=cmdline.found("debug");
	const char	*debugfile=NULL;
	if (debug) {
		debugfile=cmdline.getValue("debug");
	}

	// at least id, host or socket, and table or sequence are required
	if ((charstring::isNullOrEmpty(id) &&
		charstring::isNullOrEmpty(host) &&
		charstring::isNullOrEmpty(socket)) ||
		(charstring::isNullOrEmpty(table) &&
			charstring::isNullOrEmpty(sequence))) {

		stdoutput.printf("usage: \n"
			" %s-export -host host -port port -socket socket\n"
			"        [-user user -password password]\n"
			"        [-krb] [-krbservice svc] [-krbmech mech] "
			"[-krbflags flags]\n"
			"        [-tls] [-tlsversion version]\n"
			"        [-tlscert certfile] [-tlspassword password]\n"
			"        [-tlsciphers cipherlist]\n"
			"        [-tlsvalidate (no|ca|ca+domain|ca+host)] "
			"        [-tlsca ca] [-tlsdepth depth]\n"
			"        (-table table | -sequence sequence)\n"
			"        [-format (xml|csv)] "
			"[-resultsetbuffersize rows]\n"
			"        [-debug [filename]]\n"
			"  or\n"
			" %s-export [-config config] -id id\n"
			"        (-table table | -sequence sequence)\n"
			"        [-format (xml|csv)] "
			"[-resultsetbuffersize rows]\n"
			"        [-debug [filename]]\n",SQLR,SQLR);
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

	sqlrcur.setResultSetBufferSize(rsbs);

	bool	result=false;
	if (!charstring::isNullOrEmpty(table)) {
		result=exportTable(&sqlrcur,table,format);
	} else if (!charstring::isNullOrEmpty(sequence)) {
		result=exportSequence(&sqlrcon,&sqlrcur,sequence,format);
	}

	sqlrcon.endSession();

	process::exit((result)?0:1);
}
