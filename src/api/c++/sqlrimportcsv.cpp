// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportcsv.h>

#include <rudiments/file.h>

sqlrimportcsv::sqlrimportcsv() : sqlrimport(), csvsax() {
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	committedcount=0;
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] numbercolumn;
}

bool sqlrimportcsv::importFromFile(const char *filename) {
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
	// FIXME: describe the table, set this and use it rather than
	// calling isNumber in field() below
	numbercolumn=new bool[colcount];
	if (lg) {
		lg->write(coarseloglevel,NULL,logindent,
				"%ld columns",(unsigned long)colcount);
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
		escapeField(&query,value);
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
		if (commitcount && !rowcount) {
			sqlrcon->begin();
		}
		if (!sqlrcur->sendQuery(query.getString())) {
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
			sqlrcon->commit();
			sqlrcon->begin();
		}
		rowcount++;
		if (lg && !(rowcount%100)) {
			lg->write(fineloglevel,NULL,logindent,
					"imported %lld rows",
					(unsigned long long)rowcount);
		}
		if (commitcount && !(rowcount%commitcount)) {
			sqlrcon->commit();
			committedcount++;
			if (lg) {
				if (!(committedcount%10)) {
					lg->write(coarseloglevel,NULL,logindent,
						"committed %lld rows "
						"(to %s)...",
						(unsigned long long)rowcount,
						table);
				} else {
					lg->write(fineloglevel,NULL,logindent,
						"committed %lld rows",
						(unsigned long long)rowcount);
				}
			}
			sqlrcon->begin();
		}
	}
	return true;
}

bool sqlrimportcsv::bodyEnd() {
	sqlrcon->commit();
	if (lg) {
		lg->write(coarseloglevel,NULL,logindent,
				"  committed %lld rows (to %s)",
				(unsigned long long)rowcount,table);
	}
	return true;
}


void sqlrimportcsv::escapeField(stringbuffer *strb, const char *field) {
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
