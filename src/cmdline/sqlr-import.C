// Copyright (c) 2005  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>
#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <sqlrconfigfile.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

class sqlrimport : public xmlsax {
	public:
			sqlrimport(sqlrcursor *sqlrcur);
			~sqlrimport();
	private:
		bool	tagStart(const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	tagEnd(const char *name);

		bool	tableTagStart();
		bool	columnsTagStart();
		bool	columnTagStart();
		bool	rowsTagStart();
		bool	rowTagStart();
		bool	fieldTagStart();

		bool	tableTagEnd();
		bool	columnsTagEnd();
		bool	columnTagEnd();
		bool	rowsTagEnd();
		bool	rowTagEnd();
		bool	fieldTagEnd();

		bool	text(const char *string);

		void	unescapeField(stringbuffer *strb, const char *field);

		sqlrcursor	*sqlrcur;

		unsigned short	currenttag;
		const char	*currentattribute;

		stringbuffer	query;
		char		*table;
		uint32_t	colcount;
		stringbuffer	columns;
		bool		*numbercolumn;
		uint32_t	currentcol;
		bool		infield;
		uint32_t	fieldcount;

		static const unsigned short	NULLTAG;
		static const unsigned short	TABLETAG;
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
const unsigned short sqlrimport::COLUMNSTAG=2;
const unsigned short sqlrimport::COLUMNTAG=3;
const unsigned short sqlrimport::ROWSTAG=4;
const unsigned short sqlrimport::ROWTAG=5;
const unsigned short sqlrimport::FIELDTAG=6;

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

sqlrimport::sqlrimport(sqlrcursor *sqlrcur) : xmlsax() {
	this->sqlrcur=sqlrcur;
	currenttag=NULLTAG;
	currentattribute=NULL;
	table=NULL;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	fieldcount=0;
}

sqlrimport::~sqlrimport() {
	delete[] currentattribute;
	delete[] table;
	delete[] numbercolumn;
}

bool sqlrimport::tagStart(const char *name) {
	if (!charstring::compare(name,"table")) {
		return tableTagStart();
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
		sqlrcur->sendQuery(query.getString());
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
		if (!numbercolumn[currentcol]) {
			query.append('\'');
		}
		unescapeField(&query,string);
		if (!numbercolumn[currentcol]) {
			query.append('\'');
		}
		if (currentcol<colcount-1) {
			query.append(",");
		}
	}
	return true;
}

void sqlrimport::unescapeField(stringbuffer *strb, const char *field) {

	// FIXME: what about escaped NULL's
	for (uint32_t index=0; field[index]; index++) {
		if (field[index]=='\\') {
			index++;
		}
		strb->append(field[index]);
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

	const char	*config=cmdline.value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.value("-id");
	if (!(id && id[0])) {


		if (argc<7) {
			printf("usage: sqlr-import  host port socket "
				"user password file [debug] \n"
				"  or   sqlr-import  [-config configfile] "
				"-id id file [debug]\n");
			exit(1);
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
				if (!charstring::compare(argv[i],"debug")) {
					continue;
				}
				file=argv[i];
				break;
			}
		} else {
			exit(1);
		}
	}



	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcon.debugOn();
	sqlrcursor	sqlrcur(&sqlrcon);

	if (debug) {
		sqlrcon.debugOn();
	}

	sqlrimport	sqlri(&sqlrcur);
	exit(!sqlri.parseFile(file));
}
