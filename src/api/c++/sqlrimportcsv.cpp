// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportcsv.h>

#include <rudiments/file.h>
#include <rudiments/datetime.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_DATETIME_TYPE_CHAR
#include <datatypes.h>

sqlrimportcsv::sqlrimportcsv() : sqlrimport(), csvsax() {
	insertprimarykey=false;
	primarykeycolumnname=NULL;
	primarykeycolumnindex=0;
	primarykeysequence=NULL;
	ignorecolumnswithemptynames=false;
	ignoreemptyrows=false;
	colcount=0;
	currenttablecol=0;
	currentcol=0;
	numbercolumn=NULL;
	datecolumn=NULL;
	foundfieldtext=false;
	fieldcount=0;
	rowcount=0;
	committedcount=0;
	columnswithemptynamesnode=NULL;
	staticvaluecolumnnames.setManageArrayValues(true);
	staticvaluecolumnvalues.setManageArrayValues(true);
}

sqlrimportcsv::~sqlrimportcsv() {
	delete[] primarykeycolumnname;
	delete[] primarykeysequence;
	delete[] numbercolumn;
	delete[] datecolumn;
}

void sqlrimportcsv::insertPrimaryKey(const char *primarykeycolumnname,
					uint32_t primarykeycolumnindex,
					const char *primarykeysequence) {
	removePrimaryKey();
	this->primarykeycolumnname=charstring::duplicate(primarykeycolumnname);
	this->primarykeycolumnindex=primarykeycolumnindex;
	this->primarykeysequence=charstring::duplicate(primarykeysequence);
	insertprimarykey=true;
}

void sqlrimportcsv::removePrimaryKey() {
	delete[] this->primarykeycolumnname;
	delete[] this->primarykeysequence;
	this->primarykeycolumnname=NULL;
	this->primarykeysequence=NULL;
	insertprimarykey=false;
}

void sqlrimportcsv::insertStaticValue(const char *columnname,
					uint32_t columnindex,
					const char *value) {
	removeStaticValue(columnindex);
	staticvaluecolumnnames.setValue(
			columnindex,charstring::duplicate(columnname));
	staticvaluecolumnvalues.setValue(
			columnindex,charstring::duplicate(value));
}

void sqlrimportcsv::removeStaticValue(uint32_t columnindex) {
	staticvaluecolumnnames.remove(columnindex);
	staticvaluecolumnvalues.remove(columnindex);
}

void sqlrimportcsv::setIgnoreColumnsWithEmptyNames(
					bool ignorecolumnswithemptynames) {
	this->ignorecolumnswithemptynames=ignorecolumnswithemptynames;
}

void sqlrimportcsv::setIgnoreEmptyRows(bool ignoreemptyrows) {
	this->ignoreemptyrows=ignoreemptyrows;
}

bool sqlrimportcsv::importFromFile(const char *filename) {

	delete[] numbercolumn;
	delete[] datecolumn;
	numbercolumn=NULL;
	datecolumn=NULL;
	columnswithemptynames.clear();

	if (!objectname) {
		objectname=file::basename(filename,".csv");
	}
	return csvsax::parseFile(filename);
}

bool sqlrimportcsv::column(const char *name, bool quoted) {

	// if this column is the primary key...
	if (insertprimarykey && currentcol==primarykeycolumnindex) {

		// and we're building a list of column names from the ones
		// specified in the CSV header, rather than just grabbing
		// the columns from the table itself...
		if (!ignorecolumns) {

			// append the primary key name
			columns[columns.getLength()]=
				charstring::duplicate(primarykeycolumnname);
		}
		currentcol++;
	}

	// if there are any static columns...
	if (staticvaluecolumnnames.getLength()) {

		// loop, handling them
		for (;;) {

			// get the static column name for this position
			const char	*colname=
				staticvaluecolumnnames.getValue(currentcol);
			if (!colname) {
				break;
			}

			// and we're building a list of column names from the
			// ones specified in the CSV header, rather than just
			// grabbing the columns from the table itself...
			if (!ignorecolumns) {

				// append the column name
				columns[columns.getLength()]=
					charstring::duplicate(colname);
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

	// and we're building a list of column names from the ones specified in
	// the CSV header, rather than just grabbing the columns from the table
	// itself, and not ignoring this column because it's name was empty...
	if (!ignorecolumns && includecolumn) {

		// append the column name to the list of column names
		columns[columns.getLength()]=charstring::duplicate(name);
	}

	// next...
	currentcol++;

	return true;
}

bool sqlrimportcsv::headerEnd() {

	// we need to figure out which columns are numbers or dates...

	// bail if there were no columns
	// (eg. if the csv file was completely empty)
	if (!columns.getLength()) {
		return true;
	}

	// get info about these columns from the database
	query.clear();
	query.append("select ");

	if (ignorecolumns) {
		// if we're ignoring the columns specified in the CSV header,
		// then just grab the column names from the table itself
		query.append('*');
	} else {
		// if we built a list of column names from the ones specified in
		// the CSV header, then select those columns, specifically
		for (uint64_t i=0; i<columns.getLength(); i++) {
			if (i) {
				query.append(',');
			}
			query.append(columns[i]);
		}
	}
	query.append(" from ")->append(objectname);
	sqlrcur->setResultSetBufferSize(1);
	if (!sqlrcur->sendQuery(query.getString())) {
		return false;
	}

	// get the column count
	colcount=sqlrcur->colCount();

	// Primary key values could be provided in the CSV, but if we're
	// inserting a primary key, then we can presume that they are not
	// provided in the CSV.
	//
	// So, if we're inserting a primary key, then...
	//
	// If we ignored the columns specified in the CSV header, and just
	// grabbed the column names from the table itself, then the primary
	// key will be in the columns selected by * above.
	//
	// If we built a list of column names from the ones specified in
	// the CSV header, then the primary key will NOT be among those
	// columns, and in that case we need to bump the column count.
	if (insertprimarykey && !ignorecolumns) {
		colcount++;
	} 

	// run through the columns, figuring out which are numbers and dates...
	numbercolumn=new bool[colcount];
	datecolumn=new bool[colcount];
	// "i" is the index into the set of column names selected above
	// "j" is the index into the set number/datecolumn flags
	// they will differ if we had to bump colcount above
	for (uint32_t i=0, j=0; j<colcount;) {

		if (!ignorecolumns &&
			insertprimarykey &&
			j==primarykeycolumnindex) {

			// If we're not ignoring columns, and this is the
			// primary key column, then it will be a number and
			// not a date.
			numbercolumn[j]=true;
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

	// reset various flags and counters
	currenttablecol=0;
	currentcol=0;
	fieldcount=0;
	columnswithemptynamesnode=columnswithemptynames.getFirst();

	return true;
}

bool sqlrimportcsv::field(const char *value, bool quoted) {

	// if we're manually adding the primary key, and this is the primary
	// key position, then add it
	if (insertprimarykey && currentcol==primarykeycolumnindex) {
		if (primarykeysequence) {
			stringbuffer	tmp;
			tmp.append("nextval('");
			tmp.append(primarykeysequence);
			tmp.append("')");
			fields[fields.getLength()]=
					tmp.detachString();
		} else {
			fields[fields.getLength()]=
					charstring::duplicate("null");
		}

		// next...
		currentcol++;
		currenttablecol++;
		fieldcount++;
	}

	// if there are any static columns...
	if (staticvaluecolumnnames.getLength()) {

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

			// append the field
			stringbuffer	tmp;
			appendField(&tmp,colvalue,0,true);
			fields[fields.getLength()]=tmp.detachString();

			// next...
			currentcol++;
			currenttablecol++;
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

		// append the field
		stringbuffer	tmp;
		appendField(&tmp,value,currenttablecol,false);
		fields[fields.getLength()]=tmp.detachString();

		// next...
		currentcol++;
		currenttablecol++;
		fieldcount++;

	} else {

		// next column...
		currentcol++;
	}

	return true;
}

void sqlrimportcsv::appendField(stringbuffer *query,
					const char *value,
					uint32_t currenttablecol,
					bool overrideisstring) {

	if (!charstring::isNullOrEmpty(value)) {

		bool	isnumber=(!overrideisstring && numbercolumn)?
					numbercolumn[currenttablecol]:false;
		bool	isdate=(!overrideisstring && datecolumn)?
					datecolumn[currenttablecol]:false;
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

			// FIXME: make this configurable
			// massage the year...
			// If it's less than 100, then assume that the century
			// wasn't given.  If what was given is > 10 years from
			// the current year, then assume it was meant to be a
			// date from the previous century.
			if (year<100) {
				datetime	dt;
				dt.getSystemDateAndTime();
				int32_t	century=dt.getCentury();
				if (year>dt.getShortYear()+10) {
					century--;
				}
				year=((century-1)*100)+year;
			}

			// FIXME: what about microseconds and negatives?
			char	*dt=datetime::formatAs(
						"YYYY-MM-DD HH24:MI:SS",
						year,month,day,
						hour,minute,second,
						microsecond,isnegative);
			query->append(dt);
			delete[] dt;

		} else if (isnumber && !charstring::isNumber(value)) {
			query->append("NULL");
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

	// build query
	query.clear();
	query.append("insert into ")->append(objectname);
	if (!ignorecolumns) {
		query.append(" (");
		for (uint64_t i=0; i<columns.getLength(); i++) {
			if (i) {
				query.append(',');
			}
			const char	*c=columns[i];
			const char	*m=columnmap.getValue(c);
			if (m) {
				c=m;
			}
			char	*cm=charstring::duplicate(c);
			if (lowercasecolumnnames) {
				charstring::lower(cm);
			} else if (uppercasecolumnnames) {
				charstring::upper(cm);
			}
			query.append(cm);
			delete[] cm;
		}
		query.append(")");
	}
	query.append(" values (");
	for (uint64_t i=0; i<fields.getLength(); i++) {
		if (i) {
			query.append(',');
		}
		query.append(fields[i]);
		delete[] fields[i];
	}
	fields.clear();
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
			if (commitcount) {
				sqlrcon->commit();
				sqlrcon->begin();
			}
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
					lg->write(fineloglevel,NULL,logindent,
						"committed %lld rows "
						"(to %s)...",
						(unsigned long long)rowcount,
						objectname);
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

	if (lg) {
		lg->write(coarseloglevel,NULL,logindent,
				"imported %lld rows",
				(unsigned long long)rowcount);
	}

	// final commit
	if (commitcount) {
		sqlrcon->commit();
		if (lg) {
			lg->write(coarseloglevel,NULL,logindent,
					"committed %lld rows (to %s)",
					(unsigned long long)rowcount,
					objectname);
		}
	}

	// clean up column names
	for (uint64_t i=0; i<columns.getLength(); i++) {
		delete[] columns[i];
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
