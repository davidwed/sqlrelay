// Copyright (c) 2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <sqlrconfigfile.h>

bool exportTable(sqlrcursor *sqlrcur,
			const char *table, const char *format);
void exportTableXml(sqlrcursor *sqlrcur, const char *table);
void exportTableCsv(sqlrcursor *sqlrcur);

bool exportSequence(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
			const char *sequence, const char *format);
void exportSequenceXml(sqlrcursor *sqlrcur, const char *sequence);
void exportSequenceCsv(sqlrcursor *sqlrcur, const char *sequence);

void xmlEscapeField(const char *field, uint32_t length) {
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

void csvEscapeField(const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		// backslash-escape double quotes and ignore non-ascii
		// characters
		if (field[index]=='"') {
			stdoutput.printf("\\\"");
		} else if (field[index]>=' ' || field[index]<='~') {
			stdoutput.printf("%c",field[index]);
		}
	}
}

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
	const char	*table=cmdline.getValue("-table");
	const char	*sequence=cmdline.getValue("-sequence");
	const char	*format=cmdline.getValue("-format");
	if (!charstring::length(format)) {
		format="xml";
	}
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
		!(charstring::length(table) ||
			charstring::length(sequence))) {

		stdoutput.printf("usage: \n"
			"  sqlr-export -host host -port port -socket socket -user user -password password (-table table | -sequence sequence) [-format (xml | csv)] [-resultsetbuffersize rows] [-debug [filename]]\n"
			"    or\n"
			"  sqlr-export [-config configfile] -id id (-table table | -sequence sequence) [-format (xml | csv)] [-resultsetbuffersize rows] [-debug [filename]]\n");
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

	sqlrcur.setResultSetBufferSize(rsbs);

	bool	result=false;
	if (charstring::length(table)) {
		result=exportTable(&sqlrcur,table,format);
	} else if (charstring::length(sequence)) {
		result=exportSequence(&sqlrcon,&sqlrcur,sequence,format);
	}

	sqlrcon.endSession();

	process::exit((result)?0:1);
}

bool exportTable(sqlrcursor *sqlrcur, const char *table, const char *format) {

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

void exportTableXml(sqlrcursor *sqlrcur, const char *table) {

	// print header
	stdoutput.printf("<?xml version=\"1.0\"?>\n");
	stdoutput.printf("<!DOCTYPE table SYSTEM \"sqlr-export.dtd\">\n");

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
	for (;;) {
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
		if (sqlrcur->endOfResultSet() && row>=sqlrcur->rowCount()) {
			break;
		}
	}
	stdoutput.printf("</rows>\n");
	stdoutput.printf("</table>\n");
}

void exportTableCsv(sqlrcursor *sqlrcur) {

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

bool exportSequence(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
				const char *sequence, const char *format) {

	// the query we'll use to get the sequence depends on the database type
	const char	*dbtype=sqlrcon->identify();

	stringbuffer	query;
	if (!charstring::compare(dbtype,"firebird") ||
		!charstring::compare(dbtype,"interbase")) {
		query.append("select gen_id(")->append(sequence);
		query.append(",1) from rdb$database");
	} else if (!charstring::compare(dbtype,"oracle7") ||
			!charstring::compare(dbtype,"oracle8")) {
		query.append("select ")->append(sequence);
		query.append(".nextval from dual");
	} else if (!charstring::compare(dbtype,"postgresql")) {
		query.append("select nextval('")->append(sequence);
		query.append("')");
	} else if (!charstring::compare(dbtype,"db2")) {
		query.append("values nextval for ")->append(sequence);
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

void exportSequenceXml(sqlrcursor *sqlrcur, const char *sequence) {

	// print header
	stdoutput.printf("<?xml version=\"1.0\"?>\n");
	stdoutput.printf("<!DOCTYPE sequence SYSTEM \"sqlr-export.dtd\">\n");

	// print sequence value
	stdoutput.printf("<sequence name=\"%s\" value=\"%s\"/>\n",
			sequence,sqlrcur->getField(0,(uint32_t)0));
}

void exportSequenceCsv(sqlrcursor *sqlrcur, const char *sequence) {

	// print header
	stdoutput.printf("%s\n",sequence);

	// print sequence value
	stdoutput.printf("\"%s\"\n",sqlrcur->getField(0,(uint32_t)0));
}
