// Copyright (c) 2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <sqlrconfigfile.h>

#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int exportTable(sqlrcursor *sqlrcur, const char *table);
int exportSequence(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
						const char *sequence);

void escapeField(const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		if (field[index]=='\'') {
			printf("''");
		} else if (field[index]<' ' || field[index]>'~' ||
				field[index]=='&' || field[index]=='<' ||
				field[index]=='>') {
			printf("&%d;",(uint8_t)field[index]);
		} else {
			printf("%c",field[index]);
		}
	}
}

int main(int argc, const char **argv) {

	#include <version.h>

	sqlrconfigfile	cfgfile;
	usercontainer	*currentnode=NULL;

	commandline	cmdline(argc,argv);
	const char	*host;
	int16_t		port;
	const char	*socket;
	const char	*user;
	const char	*password;
	const char	*objecttype="";
	const char	*object="";
	bool		debug=false;

	const char	*config=cmdline.getValue("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.getValue("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: sqlr-export  host port socket "
				"user password (table|sequence) "
				"tablename [debug] \n"
				"  or   sqlr-export  [-config configfile] "
				"-id id (table|sequence) tablename [debug]\n");
			process::exit(1);
		}

		host=argv[1];
		port=charstring::toInteger(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		objecttype=argv[6];
		object=argv[7];
		if (argv[8] && !charstring::compare(argv[8],"debug")) {
			debug=true;
		}

	} else {

		if (cfgfile.parse(config,id)) {

			// get the host/port/socket/username/password
			host="localhost";
			port=cfgfile.getPort();
			socket=cfgfile.getUnixPort();
			// FIXME: this can return 0
			cfgfile.getUserList()->getDataByIndex(0,&currentnode);
			user=currentnode->getUser();
			password=currentnode->getPassword();

			// find the table and optional debug
			if (cmdline.found("debug")) {
				debug=true;
			}

			// find the table
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				objecttype=argv[i];
				object=argv[i+1];
				break;
			}
			if (!charstring::compare(argv[argc-1],"debug") &&
				charstring::compare(object,"debug")) {
				debug=true;
			}
		} else {
			process::exit(1);
		}
	}



	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	if (debug) {
		sqlrcon.debugOn();
	}

	sqlrcur.setResultSetBufferSize(100);

	int	exitval=0;
	if (!charstring::compare(objecttype,"table")) {
		exitval=exportTable(&sqlrcur,object);
	} else if (!charstring::compare(objecttype,"sequence")) {
		exitval=exportSequence(&sqlrcon,&sqlrcur,object);
	}

	sqlrcon.endSession();

	process::exit(exitval);
}

int exportTable(sqlrcursor *sqlrcur, const char *table) {

	stringbuffer	query;
	query.append("select * from ")->append(table);
	if (sqlrcur->sendQuery(query.getString())) {

		// print header
		printf("<?xml version=\"1.0\"?>\n");
		printf("<!DOCTYPE table SYSTEM \"sqlr-export.dtd\">\n");

		// print table name
		printf("<table name=\"%s\">\n",table);

		// print columns
		uint32_t	cols=sqlrcur->colCount();
		printf("<columns count=\"%d\">\n",cols);
		for (uint32_t j=0; j<cols; j++) {
			printf("	"
				"<column name=\"%s\" type=\"%s\"/>\n",
				sqlrcur->getColumnName(j),
				sqlrcur->getColumnType(j));
		}
		printf("</columns>\n");

		// print rows
		printf("<rows>\n");
		uint64_t	row=0;
		for (;;) {
			printf("	<row>\n");
			for (uint32_t col=0; col<cols; col++) {
				const char	*field=
						sqlrcur->getField(row,col);
				if (!field) {
					break;
				}
				printf("	<field>");
				escapeField(field,
					sqlrcur->getFieldLength(row,col));
				printf("</field>\n");
			}
			printf("	</row>\n");
			row++;
			if (sqlrcur->endOfResultSet() &&
					row>=sqlrcur->rowCount()) {
				break;
			}
		}
		printf("</rows>\n");
		printf("</table>\n");
		return 0;
	}

	printf("%s\n",sqlrcur->errorMessage());
	return 1;
}

int exportSequence(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
						const char *sequence) {

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
		printf("%s doesn't support sequences.\n",dbtype);
		return 1;
	}

	if (sqlrcur->sendQuery(query.getString())) {

		// print header
		printf("<?xml version=\"1.0\"?>\n");
		printf("<!DOCTYPE sequence SYSTEM \"sqlr-export.dtd\">\n");

		// print table name
		printf("<sequence name=\"%s\" value=\"%s\"/>\n",
				sequence,sqlrcur->getField(0,(uint32_t)0));
		return 0;
	}

	printf("%s\n",sqlrcur->errorMessage());
	return 1;
}
