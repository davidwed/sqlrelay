// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportcsv.h>

#include <rudiments/file.h>
#include <rudiments/datetime.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_DATETIME_TYPE_CHAR
#include <datatypes.h>

sqlrimportcsv::sqlrimportcsv() : sqlrimport(), csvsax() {
	primarykeycolumnname=NULL;
	primarykeycolumnindex=0;
	primarykeysequence=NULL;
	ignorecolumnswithemptynames=false;
	ignoreemptyrows=false;
	colcount=0;
	currentcol=0;
	numbercolumn=NULL;
	datecolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	committedcount=0;
	needcomma=false;
	columnswithemptynamesnode=NULL;
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] primarykeycolumnname;
	delete[] primarykeysequence;
	delete[] numbercolumn;
	delete[] datecolumn;
	staticvaluecolumnnames.clearAndArrayDeleteValues();
	staticvaluecolumnvalues.clearAndArrayDeleteValues();
}

void sqlrimportcsv::insertPrimaryKey(const char *primarykeycolumnname,
					uint32_t primarykeycolumnindex,
					const char *primarykeysequence) {
	delete[] this->primarykeycolumnname;
	this->primarykeycolumnname=charstring::duplicate(primarykeycolumnname);
	this->primarykeycolumnindex=primarykeycolumnindex;
	delete[] this->primarykeysequence;
	this->primarykeysequence=charstring::duplicate(primarykeysequence);
}

void sqlrimportcsv::insertStaticValue(const char *columnname,
					uint32_t columnindex,
					const char *value) {
	staticvaluecolumnnames.removeAndArrayDeleteValue(columnindex);
	staticvaluecolumnvalues.removeAndArrayDeleteValue(columnindex);
	staticvaluecolumnnames.setValue(
			columnindex,charstring::duplicate(columnname));
	staticvaluecolumnvalues.setValue(
			columnindex,charstring::duplicate(value));
}

void sqlrimportcsv::setIgnoreColumnsWithEmptyNames(
					bool ignorecolumnswithemptynames) {
	this->ignorecolumnswithemptynames=ignorecolumnswithemptynames;
}

void sqlrimportcsv::setIgnoreEmptyRows(bool ignoreemptyrows) {
	this->ignoreemptyrows=ignoreemptyrows;
}

bool sqlrimportcsv::importFromFile(const char *filename) {

	needcomma=false;

	delete[] numbercolumn;
	delete[] datecolumn;
	numbercolumn=NULL;
	datecolumn=NULL;
	columnswithemptynames.clear();

	if (!table) {
		table=file::basename(filename,".csv");
	}
	return csvsax::parseFile(filename);
}

bool sqlrimportcsv::column(const char *name, bool quoted) {

	// if this column is the primary key...
	if (primarykeycolumnname && currentcol==primarykeycolumnindex) {

		// and we're building a list of column names...
		if (!ignorecolumns) {

			if (needcomma) {
				columns.append(',');
			}

			// append the primary key name
			columns.append(primarykeycolumnname);

			needcomma=true;
		}
		currentcol++;
	}

	// if there are any static columns...
	if (staticvaluecolumnnames.getTree()->getLength()) {

		// loop, handling them
		for (;;) {

			// get the static column name for this position
			const char	*colname=
				staticvaluecolumnnames.getValue(currentcol);
			if (!colname) {
				break;
			}

			// if we're building a list of column names...
			if (!ignorecolumns) {

				if (needcomma) {
					columns.append(',');
				}

				// append the column name
				columns.append(colname);

				needcomma=true;
			}
			currentcol++;
		}
	}

	// by default, we want to include this column in the list of column
	// names that we're building
	bool	includecolumn=true;

	// but, if we're ignoring columns with empty names...
	if (ignorecolumnswithemptynames) {

		// if this column name is empty, then don't include it
		// list of column names that we're building
		includecolumn=!charstring::isNullOrEmpty(name);

		if (!includecolumn) {

			// and put it in the list of columns to ignore when
			// importing data later too
			columnswithemptynames.append(currentcol);
		}
	}

	// if we're building a list of column names and
	// not ignoring this column because it's name was empty...
	if (!ignorecolumns && includecolumn) {

		if (needcomma) {
			columns.append(',');
		}

		// append the column name to the list of column names
		columns.append(name);

		needcomma=true;
	}

	// next...
	currentcol++;

	return true;
}

bool sqlrimportcsv::headerEnd() {

	// we need to figure out which columns are numbers or dates...

	// get info about these columns from the database
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

	// get the column count
	// this should match the number of columns in the csv,
	// minus any that are being ignored because their names are empty,
	// plus a primary key if we're adding that manually
	colcount=sqlrcur->colCount();

	// If a primary key name/position is specified, then we presume that
	// the primary key isn't one of the columns in the csv.
	//
	// If we're ignoring columns, then the primary key will be in the
	// columns selected by * above.
	//
	// If we're not ignoring columns, then it will NOT be in the columns,
	// explicitly selected by name above, so we need to bump the colcount.
	if (!ignorecolumns) {
		colcount++;
	} 

	// run through the columns, figuring out which are numbers and dates...
	numbercolumn=new bool[colcount];
	datecolumn=new bool[colcount];
	for (uint32_t i=0, j=0; i<colcount;) {

		if (!ignorecolumns &&
			primarykeycolumnname &&
			i==primarykeycolumnindex) {

			// If we're not ignoring columns, and this is the
			// primary key column, then it won't be in the set of
			// columns selected by name above.  Fudge it.  It's
			// going to be a number and not a date.
			// Don't increment i.
			numbercolumn[j]=false;
			datecolumn[j]=false;

		} else {

			numbercolumn[j]=isNumberTypeChar(
						sqlrcur->getColumnType(i));
			datecolumn[j]=isDateTimeTypeChar(
						sqlrcur->getColumnType(i));
			i++;
		}
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

	// reset the insert query
	query.clear();
	query.append("insert into ")->append(table);
	if (!ignorecolumns) {
		query.append(" (")->append(columns.getString())->append(")");
	}
	query.append(" values (");

	// reset various flags and counters
	needcomma=false;
	currentcol=0;
	fieldcount=0;
	columnswithemptynamesnode=columnswithemptynames.getFirst();

	return true;
}

bool sqlrimportcsv::field(const char *value, bool quoted) {

	// if we're manually adding the primary key, and this is the primary
	// key position, then add it
	if (primarykeycolumnname && currentcol==primarykeycolumnindex) {
		if (needcomma) {
			query.append(",");
		}
		if (primarykeysequence) {
			query.append("nextval('");
			query.append(primarykeysequence);
			query.append("')");
		} else {
			query.append("null");
		}
		needcomma=true;
		currentcol++;
		fieldcount++;
	}

	// if there are any static columns...
	if (staticvaluecolumnnames.getTree()->getLength()) {

		// loop, handling them
		for (;;) {

			// get the static column name for this position
			const char	*colname=
				staticvaluecolumnnames.getValue(currentcol);
			if (!colname) {
				break;
			}

			// get the static column value for this position
			const char	*colvalue=
				staticvaluecolumnvalues.getValue(currentcol);

			if (needcomma) {
				query.append(',');
			}

			// append the field to the query
			appendField(&query,colvalue,currentcol);

			needcomma=true;
			currentcol++;
			fieldcount++;
		}
	}

	// should we include this field, or ignore it
	// because its column name was blank?
	bool	includefield=true;
	if (ignorecolumnswithemptynames &&
			columnswithemptynamesnode &&
			currentcol==columnswithemptynamesnode->getValue()) {
		includefield=false;
		columnswithemptynamesnode=columnswithemptynamesnode->getNext();
	}

	// if we should include this field...
	if (includefield) {

		if (needcomma) {
			query.append(",");
		}

		// append the field to the query
		appendField(&query,value,currentcol);

		needcomma=true;

		// next...
		currentcol++;
		fieldcount++;
	}
	return true;
}

void sqlrimportcsv::appendField(stringbuffer *query,
					const char *value,
					uint32_t currentcol) {

	if (!charstring::isNullOrEmpty(value)) {

		bool	isnumber=(numbercolumn)?numbercolumn[currentcol]:false;
		bool	isdate=(datecolumn)?datecolumn[currentcol]:false;
		if (!isnumber || isdate) {
			query->append('\'');
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
					&hour,&minute,&second,
					&microsecond,&isnegative);
			if (hour==-1) {
				hour=0;
			}
			if (minute==-1) {
				minute=0;
			}
			if (second==-1) {
				second=0;
			}
			if (microsecond==-1) {
				microsecond=0;
			}
			// FIXME: what about microseconds and negatives?
			char	*dt=datetime::formatAs(
						"YYYY-MM-DD HH24:MI:SS",
						year,month,day,
						hour,minute,second,
						microsecond,isnegative);
			query->append(dt);
			delete[] dt;
		} else {
			escapeField(query,value);
		}
		if (!isnumber || isdate) {
			query->append('\'');
		}
	} else {
		query->append("NULL");
	}
}

bool sqlrimportcsv::rowEnd() {

	// terminate the values list in the query
	query.append(')');

	// if there were any actual values (i.e. not an empty csv)
	if (fieldcount) {

		// if we're committing every so often, and this is the very
		// first row, then begin a transaction
		if (commitcount && !rowcount) {
			sqlrcon->begin();
		}

		// send the query
		if (!sqlrcur->sendQuery(query.getString())) {
			if (lg && logerrors) {
				lg->write(coarseloglevel,NULL,logindent,
						"%s",sqlrcur->errorMessage());
			}
			sqlrcon->commit();
			sqlrcon->begin();
		}

		// bump the rowcount
		rowcount++;

		// log
		if (lg && !(rowcount%100)) {
			lg->write(fineloglevel,NULL,logindent,
					"imported %lld rows",
					(unsigned long long)rowcount);
		}

		// if we're committing every so often, and it's time to commit,
		// then commit, log and begin a new transaction
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

	// final commit
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
