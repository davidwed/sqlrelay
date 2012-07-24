// Copyright (c) 2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/process.h>
#include <sqlrconfigfile.h>

#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrimport : public xmlsax {
	public:
			sqlrimport(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur);
			~sqlrimport();
	private:
		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *name);

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

		void	unescapeField(stringbuffer *strb, const char *field);

		sqlrconnection	*sqlrcon;
		sqlrcursor	*sqlrcur;

		unsigned short	currenttag;
		const char	*currentattribute;

		stringbuffer	query;
		char		*table;
		char		*sequence;
		char		*sequencevalue;
		uint32_t	colcount;
		stringbuffer	columns;
		bool		*numbercolumn;
		uint32_t	currentcol;
		bool		infield;
		uint32_t	fieldcount;

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

const unsigned short sqlrimport::NULLTAG=0;
const unsigned short sqlrimport::TABLETAG=1;
const unsigned short sqlrimport::SEQUENCETAG=2;
const unsigned short sqlrimport::COLUMNSTAG=3;
const unsigned short sqlrimport::COLUMNTAG=4;
const unsigned short sqlrimport::ROWSTAG=5;
const unsigned short sqlrimport::ROWTAG=6;
const unsigned short sqlrimport::FIELDTAG=7;

const unsigned short sqlrimport::NULLATTR=0;
const unsigned short sqlrimport::NAMEATTR=1;
const unsigned short sqlrimport::TYPEATTR=3;
const unsigned short sqlrimport::LENGTHATTR=4;
const unsigned short sqlrimport::PRECISIONATTR=5;
const unsigned short sqlrimport::SCALEATTR=6;
const unsigned short sqlrimport::NULLABLEATTR=7;
const unsigned short sqlrimport::PRIMARYKEYATTR=8;
const unsigned short sqlrimport::UNIQUEATTR=9;
const unsigned short sqlrimport::PARTOFKEYATTR=10;
const unsigned short sqlrimport::UNSIGNEDATTR=11;
const unsigned short sqlrimport::ZEROFILLEDATTR=12;
const unsigned short sqlrimport::BINARYATTR=13;
const unsigned short sqlrimport::AUTOINCREMENTATTR=14;

sqlrimport::sqlrimport(sqlrconnection *sqlrcon,
			sqlrcursor *sqlrcur) : xmlsax() {
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
	fieldcount=0;
}

sqlrimport::~sqlrimport() {
	delete[] currentattribute;
	delete[] table;
	delete[] sequence;
	delete[] sequencevalue;
	delete[] numbercolumn;
}

bool sqlrimport::tagStart(const char *name) {
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

bool sqlrimport::attributeName(const char *name) {
	delete[] currentattribute;
	currentattribute=charstring::duplicate(name);
	return true;
}

bool sqlrimport::attributeValue(const char *value) {
	switch (currenttag) {
		case TABLETAG:
			if (!charstring::compare(currentattribute,"name")) {
				delete[] table;
				table=charstring::duplicate(value);
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

bool sqlrimport::tagEnd(const char *name) {
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

bool sqlrimport::tableTagStart() {
	currenttag=TABLETAG;
	return true;
}

bool sqlrimport::sequenceTagStart() {
	currenttag=SEQUENCETAG;
	return true;
}

bool sqlrimport::columnsTagStart() {
	currenttag=COLUMNSTAG;
	return true;
}

bool sqlrimport::columnTagStart() {
	currenttag=COLUMNTAG;
	return true;
}

bool sqlrimport::rowsTagStart() {
	currenttag=ROWSTAG;
	return true;
}

bool sqlrimport::rowTagStart() {
	query.clear();
	query.append("insert into ")->append(table)->append(" (");
	query.append(columns.getString())->append(") values (");
	currenttag=ROWTAG;
	currentcol=0;
	fieldcount=0;
	return true;
}

bool sqlrimport::fieldTagStart() {
	currenttag=FIELDTAG;
	infield=true;
	return true;
}


bool sqlrimport::tableTagEnd() {
	return true;
}

bool sqlrimport::sequenceTagEnd() {

	const char	*dbtype=sqlrcon->identify();
	query.clear();

	// sqlite, mysql, sybase/mssql have autoincrementing fields
	// mdbtools has nothing
	// odbc can't tell what kind of underlying db we're using
	if (!charstring::compare(dbtype,"firebird") ||
		!charstring::compare(dbtype,"interbase")) {
		query.append("set generator ")->append(sequence);
		query.append(" to ")->append(sequencevalue);
		if (!sqlrcur->sendQuery(query.getString())) {
			printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	} else if (!charstring::compare(dbtype,"oracle7") ||
			!charstring::compare(dbtype,"oracle8")) {
		sqlrcursor	sqlrcur2(sqlrcon);
		char	*uppersequence=charstring::duplicate(sequence);
		charstring::upper(uppersequence);
		query.append("select * from all_sequences where "
				"sequence_name='");
		query.append(uppersequence)->append("'");
		delete[] uppersequence;
		if (!sqlrcur2.sendQuery(query.getString())) {
			printf("%s\n",sqlrcur->errorMessage());
			return true;
		}
		query.clear();
		query.append("drop sequence ")->append(sequence);
		if (!sqlrcur->sendQuery(query.getString())) {
			printf("%s\n",sqlrcur->errorMessage());
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
			printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	} else if (!charstring::compare(dbtype,"postgresql") ||
			!charstring::compare(dbtype,"db2")) {
		query.append("alter sequence ")->append(sequence);
		query.append(" restart with ")->append(sequencevalue);
		if (!sqlrcur->sendQuery(query.getString())) {
			printf("%s\n",sqlrcur->errorMessage());
		}
		return true;
	}

	printf("%s doesn't support sequences.\n",dbtype);
	return true;
}

bool sqlrimport::columnsTagEnd() {
	return true;
}

bool sqlrimport::columnTagEnd() {
	currentcol++;
	return true;
}

bool sqlrimport::rowsTagEnd() {
	return true;
}

bool sqlrimport::rowTagEnd() {
	query.append(')');
	if (fieldcount) {
		if (!sqlrcur->sendQuery(query.getString())) {
			printf("%s\n",sqlrcur->errorMessage());
		}
	}
	return true;
}

bool sqlrimport::fieldTagEnd() {
	infield=false;
	currentcol++;
	fieldcount++;
	return true;
}

bool sqlrimport::text(const char *string) {
	if (infield) {
		if (charstring::length(string)) {
			if (!numbercolumn[currentcol]) {
				query.append('\'');
			}
			unescapeField(&query,string);
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

void sqlrimport::unescapeField(stringbuffer *strb, const char *field) {

	for (uint32_t index=0; field[index]; index++) {
		if (field[index]=='&') {
			index++;
			strb->append((char)charstring::
					toUnsignedInteger(field+index));
			while (field[index] && field[index]!=';') {
				index++;
			}
			if (!field[index]) {
				break;
			}
		} else {
			strb->append(field[index]);
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
	const char	*file="";
	bool		debug=false;

	const char	*config=cmdline.getValue("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.getValue("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: sqlr-import  host port socket "
				"user password file [debug] \n"
				"  or   sqlr-import  [-config configfile] "
				"-id id file [debug]\n");
			process::exit(1);
		}

		host=argv[1];
		port=charstring::toInteger(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		file=argv[6];
		if (argv[7] && !charstring::compare(argv[7],"debug")) {
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

			// find the file and optional debug
			if (cmdline.found("debug")) {
				debug=true;
			}

			// find the file
			for (int i=1; i<argc; i++) {
				if (argv[i][0]=='-') {
					i++;
					continue;
				}
				file=argv[i];
				break;
			}
			if (!charstring::compare(argv[argc-1],"debug") &&
				charstring::compare(file,"debug")) {
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

	sqlrimport	sqlri(&sqlrcon,&sqlrcur);
	process::exit(!sqlri.parseFile(file));
}
