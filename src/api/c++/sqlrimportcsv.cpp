// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportcsv.h>

#include <rudiments/file.h>

sqlrimportcsv::sqlrimportcsv(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur,
				uint64_t commitcount,
				bool verbose,
				const char *dbtype) : csvsax() {
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	table=NULL;
	ignorecolumns=false;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	this->verbose=verbose;
	this->commitcount=commitcount;
	committedcount=0;
	this->dbtype=dbtype;
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] table;
	delete[] numbercolumn;
}

void sqlrimportcsv::setTable(const char *table) {
	delete[] this->table;
	this->table=charstring::duplicate(table);
}

void sqlrimportcsv::setIgnoreColumns(bool ignorecolumns) {
	this->ignorecolumns=ignorecolumns;
}

bool sqlrimportcsv::parseFile(const char *filename) {
	if (!table) {
		table=file::basename(filename,".csv");
	}
	return csvsax::parseFile(filename);
}

bool sqlrimportcsv::column(const char *name, bool quoted) {
	if (!ignorecolumns) {
		if (colcount) {
			columns.append(',');
		}
		columns.append(name);
		colcount++;
	}
	return true;
}

bool sqlrimportcsv::headerEnd() {
	// FIXME: set this and use it rather than
	// calling isNumber in field() below
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
	query.append("insert into ")->append(table);
	if (colcount) {
		query.append(" (")->append(columns.getString())->append(")");
	}
	query.append(" values (");
	currentcol=0;
	fieldcount=0;
	return true;
}

bool sqlrimportcsv::field(const char *value, bool quoted) {
	if (currentcol) {
		query.append(",");
	}
	if (!charstring::isNullOrEmpty(value)) {
		// FIXME: get column type and use that intead of isNumber
		bool	isnumber=charstring::isNumber(value);
		if (!isnumber) {
			query.append('\'');
		}
		massageField(&query,value);
		if (!isnumber) {
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
			if (verbose) {
				stdoutput.printf("%s\n",
					sqlrcur->errorMessage());
			}
			sqlrcon->commit();
			sqlrcon->begin();
		}
		rowcount++;
		if (verbose && !(rowcount%100)) {
			stdoutput.printf("  imported %lld rows",
					(unsigned long long)rowcount);
		}
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
