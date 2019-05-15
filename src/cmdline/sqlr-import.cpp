// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <sqlrelay/sqlrutil.h>
#include <rudiments/xmlsax.h>
#include <rudiments/csvsax.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>
#include <config.h>
#include <defaults.h>
#define NEED_IS_NUMBER_TYPE_CHAR 1
#include <datatypes.h>
#include <version.h>

class sqlrimportxml : public xmlsax {
	public:
			sqlrimportxml(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					uint64_t commitcount,
					bool verbose,
					const char *dbtype);
			~sqlrimportxml();
	private:
		bool	tagStart(const char *ns, const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *ns, const char *name);

		bool	tableTagStart();
		bool	sequenceTagStart();
		bool	columnsTagStart();
		bool	columnTagStart();
		bool	rowsTagStart();
		bool	rowTagStart();
		bool	fieldTagStart();

		bool	tableTagEnd();
		bool	sequenceTagEnd();
		bool	columnsTagEnd();
		bool	columnTagEnd();
		bool	rowsTagEnd();
		bool	rowTagEnd();
		bool	fieldTagEnd();

		bool	text(const char *string);

		void	massageField(stringbuffer *strb, const char *field);

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;

		unsigned short	currenttag;
		char		*currentattribute;

		stringbuffer	query;
		char		*table;
		char		*sequence;
		char		*sequencevalue;
		uint32_t	colcount;
		stringbuffer	columns;
		bool		*numbercolumn;
		uint32_t	currentcol;
		bool		infield;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	commitcount;
		uint64_t	committedcount;
		bool		verbose;
		const char	*dbtype;

		static const unsigned short	NULLTAG;
		static const unsigned short	TABLETAG;
		static const unsigned short	SEQUENCETAG;
		static const unsigned short	COLUMNSTAG;
		static const unsigned short	COLUMNTAG;
		static const unsigned short	ROWSTAG;
		static const unsigned short	ROWTAG;
		static const unsigned short	FIELDTAG;

		static const unsigned short	NULLATTR;
		static const unsigned short	NAMEATTR;
		static const unsigned short	TYPEATTR;
		static const unsigned short	LENGTHATTR;
		static const unsigned short	PRECISIONATTR;
		static const unsigned short	SCALEATTR;
		static const unsigned short	NULLABLEATTR;
		static const unsigned short	PRIMARYKEYATTR;
		static const unsigned short	UNIQUEATTR;
		static const unsigned short	PARTOFKEYATTR;
		static const unsigned short	UNSIGNEDATTR;
		static const unsigned short	ZEROFILLEDATTR;
		static const unsigned short	BINARYATTR;
		static const unsigned short	AUTOINCREMENTATTR;
};

const unsigned short sqlrimportxml::NULLTAG=0;
const unsigned short sqlrimportxml::TABLETAG=1;
const unsigned short sqlrimportxml::SEQUENCETAG=2;
const unsigned short sqlrimportxml::COLUMNSTAG=3;
const unsigned short sqlrimportxml::COLUMNTAG=4;
const unsigned short sqlrimportxml::ROWSTAG=5;
const unsigned short sqlrimportxml::ROWTAG=6;
const unsigned short sqlrimportxml::FIELDTAG=7;

const unsigned short sqlrimportxml::NULLATTR=0;
const unsigned short sqlrimportxml::NAMEATTR=1;
const unsigned short sqlrimportxml::TYPEATTR=3;
const unsigned short sqlrimportxml::LENGTHATTR=4;
const unsigned short sqlrimportxml::PRECISIONATTR=5;
const unsigned short sqlrimportxml::SCALEATTR=6;
const unsigned short sqlrimportxml::NULLABLEATTR=7;
const unsigned short sqlrimportxml::PRIMARYKEYATTR=8;
const unsigned short sqlrimportxml::UNIQUEATTR=9;
const unsigned short sqlrimportxml::PARTOFKEYATTR=10;
const unsigned short sqlrimportxml::UNSIGNEDATTR=11;
const unsigned short sqlrimportxml::ZEROFILLEDATTR=12;
const unsigned short sqlrimportxml::BINARYATTR=13;
const unsigned short sqlrimportxml::AUTOINCREMENTATTR=14;

sqlrimportxml::sqlrimportxml(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur,
				uint64_t commitcount,
				bool verbose,
				const char *dbtype) : xmlsax() {
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	currenttag=NULLTAG;
	currentattribute=NULL;
	table=NULL;
	sequence=NULL;
	sequencevalue=NULL;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	infield=false;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	this->verbose=verbose;
	this->commitcount=commitcount;
	if (!this->commitcount) {
		this->commitcount=100;
	}
	committedcount=0;
	this->dbtype=dbtype;
}

sqlrimportxml::~sqlrimportxml() {
	delete[] currentattribute;
	delete[] table;
	delete[] sequence;
	delete[] sequencevalue;
	delete[] numbercolumn;
}

bool sqlrimportxml::tagStart(const char *ns, const char *name) {
	if (!charstring::compare(name,"table")) {
		return tableTagStart();
	} else if (!charstring::compare(name,"sequence")) {
		return sequenceTagStart();
	} else if (!charstring::compare(name,"columns")) {
		return columnsTagStart();
	} else if (!charstring::compare(name,"column")) {
		return columnTagStart();
	} else if (!charstring::compare(name,"rows")) {
		return rowsTagStart();
	} else if (!charstring::compare(name,"row")) {
		return rowTagStart();
	} else if (!charstring::compare(name,"field")) {
		return fieldTagStart();
	}
	return true;
}

bool sqlrimportxml::attributeName(const char *name) {
	delete[] currentattribute;
	currentattribute=charstring::duplicate(name);
	return true;
}

bool sqlrimportxml::attributeValue(const char *value) {
	switch (currenttag) {
		case TABLETAG:
			if (!charstring::compare(currentattribute,"name")) {
				delete[] table;
				table=charstring::duplicate(value);
				if (verbose) {
					stdoutput.printf(
						"inserting into %s...\n",table);
				}
			}
			break;
		case SEQUENCETAG:
			if (!charstring::compare(currentattribute,"name")) {
				delete[] sequence;
				sequence=charstring::duplicate(value);
			}
			if (!charstring::compare(currentattribute,"value")) {
				delete[] sequencevalue;
				sequencevalue=charstring::duplicate(value);
			}
			break;
		case COLUMNSTAG:
			if (!charstring::compare(currentattribute,"count")) {
				colcount=charstring::toUnsignedInteger(value);
				columns.clear();
				delete[] numbercolumn;
				numbercolumn=new bool[colcount];
				currentcol=0;
			}
			break;
		case COLUMNTAG:
			if (!charstring::compare(currentattribute,"name")) {
				columns.append(value);
				if (currentcol<colcount-1) {
					columns.append(',');
				}
			} else if (!charstring::compare(currentattribute,
								"type")) {
				numbercolumn[currentcol]=
					isNumberTypeChar(value);
			}
			break;
		case ROWSTAG:
			break;
		case ROWTAG:
			break;
		case FIELDTAG:
			break;
	}
	return true;
}

bool sqlrimportxml::tagEnd(const char *ns, const char *name) {
	if (!charstring::compare(name,"table")) {
		return tableTagEnd();
	} else if (!charstring::compare(name,"sequence")) {
		return sequenceTagEnd();
	} else if (!charstring::compare(name,"columns")) {
		return columnsTagEnd();
	} else if (!charstring::compare(name,"column")) {
		return columnTagEnd();
	} else if (!charstring::compare(name,"rows")) {
		return rowsTagEnd();
	} else if (!charstring::compare(name,"row")) {
		return rowTagEnd();
	} else if (!charstring::compare(name,"field")) {
		return fieldTagEnd();
	}
	return true;
}

bool sqlrimportxml::tableTagStart() {
	currenttag=TABLETAG;
	rowcount=0;
	committedcount=0;
	return true;
}

bool sqlrimportxml::sequenceTagStart() {
	currenttag=SEQUENCETAG;
	return true;
}

bool sqlrimportxml::columnsTagStart() {
	currenttag=COLUMNSTAG;
	return true;
}

bool sqlrimportxml::columnTagStart() {
	currenttag=COLUMNTAG;
	return true;
}

bool sqlrimportxml::rowsTagStart() {
	currenttag=ROWSTAG;
	return true;
}

bool sqlrimportxml::rowTagStart() {
	query.clear();
	query.append("insert into ")->append(table)->append(" (");
	query.append(columns.getString())->append(") values (");
	currenttag=ROWTAG;
	currentcol=0;
	fieldcount=0;
	return true;
}

bool sqlrimportxml::fieldTagStart() {
	currenttag=FIELDTAG;
	infield=true;
	foundfieldtext=false;
	return true;
}


bool sqlrimportxml::tableTagEnd() {
	sqlrcon->commit();
	if (verbose) {
		stdoutput.printf("  committed %lld rows (to %s).\n\n",
					(unsigned long long)rowcount,table);
	}
	return true;
}

bool sqlrimportxml::sequenceTagEnd() {

	query.clear();

	// sqlite, mysql, sap/sybase and mssql have autoincrementing fields
	// mdbtools has nothing
	// odbc can't tell what kind of underlying db we're using
	if (charstring::contains(dbtype,"firebird") ||
		charstring::contains(dbtype,"interbase")) {
		query.append("set generator ")->append(sequence);
		query.append(" to ")->append(sequencevalue);
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	} else if (charstring::contains(dbtype,"oracle")) {
		sqlrcursor	sqlrcur2(sqlrcon);
		char	*uppersequence=charstring::duplicate(sequence);
		charstring::upper(uppersequence);
		query.append("select * from all_sequences where "
				"sequence_name='");
		query.append(uppersequence)->append("'");
		delete[] uppersequence;
		if (!sqlrcur2.sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
			return true;
		}
		query.clear();
		query.append("drop sequence ")->append(sequence);
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
			return true;
		}
		query.clear();
		query.append("create sequence ")->append(sequence);
		query.append(" start with ")->append(sequencevalue);
		query.append(" maxvalue ");
		query.append(sqlrcur2.getField(0,"MAX_VALUE"));
		query.append(" minvalue ");
		query.append(sqlrcur2.getField(0,"MIN_VALUE"));
		if (!charstring::compare(
				sqlrcur2.getField(0,"CYCLE_FLAG"),"N")) {
			query.append(" nocycle ");
		} else {
			query.append(" cycle ");
		}
		if (!charstring::compare(
				sqlrcur2.getField(0,"ORDER_FLAG"),"N")) {
			query.append(" noorder ");
		} else {
			query.append(" order ");
		}
		if (!charstring::compare(
				sqlrcur2.getField(0,"CACHE_SIZE"),"0")) {
			query.append(" nocache ");
		} else {
			query.append(" cache ");
			query.append(sqlrcur2.getField(0,"CACHE_SIZE"));
		}
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	} else if (charstring::contains(dbtype,"postgresql") ||
			charstring::contains(dbtype,"db2") ||
			charstring::contains(dbtype,"informix")) {
		query.append("alter sequence ")->append(sequence);
		query.append(" restart with ")->append(sequencevalue);
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	}

	stdoutput.printf("%s doesn't support sequences.\n",dbtype);
	return true;
}

bool sqlrimportxml::columnsTagEnd() {
	if (verbose) {
		stdoutput.printf("  %ld columns.\n",(unsigned long)currentcol);
	}
	return true;
}

bool sqlrimportxml::columnTagEnd() {
	currentcol++;
	return true;
}

bool sqlrimportxml::rowsTagEnd() {
	return true;
}

bool sqlrimportxml::rowTagEnd() {
	query.append(')');
	if (fieldcount) {
		if (rowcount==0) {
			sqlrcon->begin();
		}
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
		}
		rowcount++;
		if (commitcount && !(rowcount%commitcount)) {
			sqlrcon->commit();
			committedcount++;
			if (verbose) {
				stdoutput.printf("  committed %lld rows",
						(unsigned long long)rowcount);
				if (!(committedcount%10)) {
					stdoutput.printf(" (to %s)...\n",table);
				} else {
					stdoutput.printf("\n");
				}
			}
			sqlrcon->begin();
		}
	}
	return true;
}

bool sqlrimportxml::fieldTagEnd() {
	if (!foundfieldtext) {
		query.append("NULL");
		if (currentcol<colcount-1) {
			query.append(",");
		}
	}
	infield=false;
	currentcol++;
	fieldcount++;
	return true;
}

bool sqlrimportxml::text(const char *string) {
	if (infield) {
		foundfieldtext=true;
		if (!charstring::isNullOrEmpty(string)) {
			if (!numbercolumn[currentcol]) {
				query.append('\'');
			}
			massageField(&query,string);
			if (!numbercolumn[currentcol]) {
				query.append('\'');
			}
		} else {
			query.append("NULL");
		}
		if (currentcol<colcount-1) {
			query.append(",");
		}
	}
	return true;
}

void sqlrimportxml::massageField(stringbuffer *strb, const char *field) {

	for (uint32_t index=0; field[index]; index++) {
		if (field[index]=='&') {

			// expand xml entities...

			char	ch=(char)charstring::
					toUnsignedInteger(field+index);

			// double-up any single-quotes
			if (ch=='\'') {
				strb->append('\'');
			}

			index++;
			strb->append(ch);
			while (field[index] && field[index]!=';') {
				index++;
			}
			if (!field[index]) {
				break;
			}

		} else if (field[index]=='\\' &&
				(!charstring::compare(dbtype,"postgresql") ||
				!charstring::compare(dbtype,"mysql"))) {

			// for postgres and mysql, escape \'s
			strb->append("\\\\");

		} else {

			char	ch=field[index];

			// double-up any single-quotes
			if (ch=='\'') {
				strb->append('\'');
			}

			// append the character
			strb->append(ch);
		}
	}
}

class sqlrimportcsv : public csvsax {
	public:
			sqlrimportcsv(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur,
					uint64_t commitcount,
					bool verbose,
					const char *dbtype);
			~sqlrimportcsv();

		bool	parseFile(const char *filename);
	private:
		bool	column(const char *name, bool quoted);
		bool	headerEnd();
		bool	bodyStart();
		bool	rowStart();
		bool	field(const char *value, bool quoted);
		bool	rowEnd();
		bool	bodyEnd();

		void	massageField(stringbuffer *strb, const char *field);

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;

		stringbuffer	query;
		char		*table;
		uint32_t	colcount;
		stringbuffer	columns;
		bool		*numbercolumn;
		uint32_t	currentcol;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	commitcount;
		uint64_t	committedcount;
		bool		verbose;
		const char	*dbtype;
};

sqlrimportcsv::sqlrimportcsv(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur,
				uint64_t commitcount,
				bool verbose,
				const char *dbtype) : csvsax() {
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	table=NULL;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	this->verbose=verbose;
	this->commitcount=commitcount;
	if (!this->commitcount) {
		this->commitcount=100;
	}
	committedcount=0;
	this->dbtype=dbtype;
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] table;
	delete[] numbercolumn;
}

bool sqlrimportcsv::parseFile(const char *filename) {
	table=file::basename(filename,".csv");
	return csvsax::parseFile(filename);
}

bool sqlrimportcsv::column(const char *name, bool quoted) {
	if (colcount) {
		columns.append(',');
	}
	columns.append(name);
	colcount++;
	return true;
}

bool sqlrimportcsv::headerEnd() {
	numbercolumn=new bool[colcount];
	if (verbose) {
		stdoutput.printf("  %ld columns.\n",(unsigned long)colcount);
	}
	return true;
}

bool sqlrimportcsv::bodyStart() {
	rowcount=0;
	committedcount=0;
	return true;
}

bool sqlrimportcsv::rowStart() {
	query.clear();
	query.append("insert into ")->append(table)->append(" (");
	query.append(columns.getString())->append(") values (");
	currentcol=0;
	fieldcount=0;
	return true;
}

bool sqlrimportcsv::field(const char *value, bool quoted) {
	if (currentcol) {
		query.append(",");
	}
	if (!charstring::isNullOrEmpty(value)) {
		bool	numbercolumn=isNumberTypeChar(value);
		if (!numbercolumn) {
			query.append('\'');
		}
		massageField(&query,value);
		if (!numbercolumn) {
			query.append('\'');
		}
	} else {
		query.append("NULL");
	}
	currentcol++;
	fieldcount++;
	return true;
}

bool sqlrimportcsv::rowEnd() {
	query.append(')');
	if (fieldcount) {
		if (rowcount==0) {
			sqlrcon->begin();
		}
		if (!sqlrcur->sendQuery(query.getString())) {
			stdoutput.printf("%s\n",sqlrcur->errorMessage());
		}
		rowcount++;
		if (commitcount && !(rowcount%commitcount)) {
			sqlrcon->commit();
			committedcount++;
			if (verbose) {
				stdoutput.printf("  committed %lld rows",
						(unsigned long long)rowcount);
				if (!(committedcount%10)) {
					stdoutput.printf(" (to %s)...\n",table);
				} else {
					stdoutput.printf("\n");
				}
			}
			sqlrcon->begin();
		}
	}
	return true;
}

bool sqlrimportcsv::bodyEnd() {
	sqlrcon->commit();
	if (verbose) {
		stdoutput.printf("  committed %lld rows (to %s).\n\n",
					(unsigned long long)rowcount,table);
	}
	return true;
}


void sqlrimportcsv::massageField(stringbuffer *strb, const char *field) {
	for (uint32_t index=0; field[index]; index++) {
		if (field[index]=='\\' &&
				(!charstring::compare(dbtype,"postgresql") ||
				!charstring::compare(dbtype,"mysql"))) {

			// for postgres and mysql, escape \'s
			strb->append("\\\\");

		} else {

			char	ch=field[index];

			// double-up any single-quotes
			if (ch=='\'') {
				strb->append('\'');
			}

			// append the character
			strb->append(ch);
		}
	}
}

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
	uint64_t	commitcount=charstring::toInteger(
					cmdline.getValue("commitcount"));
	bool		debug=cmdline.found("debug");
	const char	*debugfile=NULL;
	if (debug) {
		debugfile=cmdline.getValue("debug");
	}
	bool		verbose=cmdline.found("verbose");

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

	// xml or csv
	const char	*suffix=charstring::findLast(file,'.');
	if (!charstring::compare(suffix,".csv")) {
		sqlrimportcsv	sqlricsv(&sqlrcon,&sqlrcur,commitcount,
						verbose,sqlrcon.identify());
		process::exit(!sqlricsv.parseFile(file));
	} else {
		sqlrimportxml	sqlrixml(&sqlrcon,&sqlrcur,commitcount,
						verbose,sqlrcon.identify());
		process::exit(!sqlrixml.parseFile(file));
	}
}
