// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportcsv.h>

#include <rudiments/file.h>
#include <rudiments/datetime.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_DATETIME_TYPE_CHAR
#include <datatypes.h>

sqlrimportcsv::sqlrimportcsv() : sqlrimport(), csvsax() {
	primarykeyname=NULL;
	primarykeyposition=0;
	primarykeysequence=NULL;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	datecolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	committedcount=0;
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] primarykeyname;
	delete[] primarykeysequence;
	delete[] numbercolumn;
	delete[] datecolumn;
}

void sqlrimportcsv::setPrimaryKeyName(const char *primarykeyname) {
	delete[] this->primarykeyname;
	this->primarykeyname=charstring::duplicate(primarykeyname);
}

void sqlrimportcsv::setPrimaryKeyPosition(uint32_t primarykeyposition) {
	this->primarykeyposition=primarykeyposition;
}

void sqlrimportcsv::setPrimaryKeySequence(const char *primarykeysequence) {
	delete[] this->primarykeysequence;
	this->primarykeysequence=charstring::duplicate(primarykeysequence);
}

bool sqlrimportcsv::importFromFile(const char *filename) {
	if (!table) {
		table=file::basename(filename,".csv");
	}
	return csvsax::parseFile(filename);
}

bool sqlrimportcsv::column(const char *name, bool quoted) {
	if (!ignorecolumns) {
		if (currentcol) {
			columns.append(',');
		}
		if (primarykeyname && currentcol==primarykeyposition) {
			columns.append(primarykeyname)->append(',');
		}
		columns.append(name);
		currentcol++;
	}
	return true;
}

bool sqlrimportcsv::headerEnd() {

	// get column info
	query.clear();
	query.append("select ");
	if (ignorecolumns) {
		query.append('*');
	} else {
		query.append(columns.getString());
	}
	query.append(" from ")->append(table);
	sqlrcur->setResultSetBufferSize(1);
	if (!sqlrcur->sendQuery(query.getString())) {
		return false;
	}
	colcount=sqlrcur->colCount();
	numbercolumn=new bool[colcount];
	datecolumn=new bool[colcount];
	for (uint32_t i=0, j=0; i<colcount; i++) {
		// if this is the primary key column then skip it
		// (and don't increment j)
		if (ignorecolumns && primarykeyname && i==primarykeyposition) {
			continue;
		}
		numbercolumn[j]=isNumberTypeChar(sqlrcur->getColumnType(i));
		datecolumn[j]=isDateTimeTypeChar(sqlrcur->getColumnType(i));
		j++;
	}
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
	if (!ignorecolumns) {
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
	if (primarykeyname && currentcol==primarykeyposition) {
		if (primarykeysequence) {
			query.append("nextval('");
			query.append(primarykeysequence);
			query.append("'),");
		} else {
			query.append("null,");
		}
	}
	if (!charstring::isNullOrEmpty(value)) {
		bool	isnumber=numbercolumn[currentcol];
		bool	isdate=datecolumn[currentcol];
		if (!isnumber || isdate) {
			query.append('\'');
		}
		if (isdate) {
			int16_t year;
			int16_t month;
			int16_t day;
			int16_t hour;
			int16_t minute;
			int16_t second;
			int32_t microsecond;
			bool isnegative;
			// FIXME: pass in ddmm, yyyyddmm, datedelimiters
			datetime::parse(value,false,false,"-/",
					&year,&month,&day,
					&hour,&minute,&second,&microsecond,
					&isnegative);
			// FIXME: what about microseconds and negatives?
			char	*dt=datetime::formatAs(
						"YYYY-MM-DD HH24:MI:SS",
						year,month,day,
						hour,minute,second,
						microsecond,isnegative);
			query.append(dt);
			delete[] dt;
		} else {
			escapeField(&query,value);
		}
		if (!isnumber || isdate) {
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
				"committed %lld rows (to %s)",
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
