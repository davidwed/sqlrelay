// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportxml.h>
#include <rudiments/stdio.h>
#define NEED_IS_NUMBER_TYPE_CHAR 1
#include <datatypes.h>

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

sqlrimportxml::sqlrimportxml() : sqlrimport(), xmlsax() {
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
	committedcount=0;
}

sqlrimportxml::~sqlrimportxml() {
	delete[] currentattribute;
	delete[] sequence;
	delete[] sequencevalue;
	delete[] numbercolumn;
}

bool sqlrimportxml::parseFile(const char *filename) {
	return xmlsax::parseFile(filename);
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
	if (commitcount) {
		sqlrcon->commit();
		if (lg) {
			lg->write(coarseloglevel,NULL,logindent,
					"committed %lld rows (to %s)",
					(unsigned long long)rowcount,table);
		}
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
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
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
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
			return true;
		}
		query.clear();
		query.append("drop sequence ")->append(sequence);
		if (!sqlrcur->sendQuery(query.getString())) {
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
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
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
		}
		return true;
	} else if (charstring::contains(dbtype,"postgresql") ||
			charstring::contains(dbtype,"db2") ||
			charstring::contains(dbtype,"informix")) {
		query.append("alter sequence ")->append(sequence);
		query.append(" restart with ")->append(sequencevalue);
		if (!sqlrcur->sendQuery(query.getString())) {
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
		}
		return true;
	}

	if (lg) {
		lg->write(coarseloglevel,NULL,logindent,
				"%s doesn't support sequences",dbtype);
	}
	return true;
}

bool sqlrimportxml::columnsTagEnd() {
	if (lg) {
		lg->write(coarseloglevel,NULL,logindent,
				"%ld columns",(unsigned long)currentcol);
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
		if (commitcount && !rowcount) {
			sqlrcon->begin();
		}
		if (!sqlrcur->sendQuery(query.getString())) {
			if (lg) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
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
					lg->write(fineloglevel,NULL,logindent,
						"committed %lld rows "
						"(to %s)...",
						(unsigned long long)rowcount);
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
