// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrexportcsv.h>
#include <sqlrelay/sqlrexportxml.h>
#include <sqlrelay/sqlrexporttable.h>
#include <sqlrelay/sqlrimportcsv.h>
#include <sqlrelay/sqlrimportxml.h>
#define NEED_IS_NUMBER_TYPE_CHAR 1
#define NEED_IS_DATETIME_TYPE_CHAR 1
#include "../../src/common/datatypes.h"
#include <rudiments/process.h>
#include <rudiments/sys.h>
#include <rudiments/file.h>
#include <rudiments/filesystem.h>
#include <rudiments/permissions.h>
#include <rudiments/dictionary.h>
#include <rudiments/randomnumber.h>
#include <rudiments/dynamicarray.h>
#include <rudiments/stdio.h>

#include "../../config.h"

//#define ROWS 10000
//#define ROWS 1000
//#define ROWS 100
#define ROWS 10
//#define ROWS 2
//#define ROWS 1

//#define DEBUG_EXPORT 1
//#define DEBUG_IMPORT 1

stringbuffer	createquery;
sqlrconnection	*con;
sqlrcursor	*cur;

void checkSuccess(const char *value, const char *success) {

	if (!success) {
		if (!value) {
			stdoutput.printf("success ");
			stdoutput.flush();
			return;
		} else {
			stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
			stdoutput.printf("failure: %s",cur->errorMessage());
			stdoutput.flush();
			delete cur;
			delete con;
			process::exit(1);
		}
	}

	if (!charstring::compare(value,success)) {
		stdoutput.printf("success ");
		stdoutput.flush();
	} else {
		stdoutput.printf("\"%s\"!=\"%s\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		stdoutput.flush();
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(int value, int success) {

	if (value==success) {
		stdoutput.printf("success ");
		stdoutput.flush();
	} else {
		stdoutput.printf("\"%d\"!=\"%d\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		stdoutput.flush();
		delete cur;
		delete con;
		process::exit(1);
	}
}

void checkSuccess(uint64_t value, uint64_t success) {

	if (value==success) {
		stdoutput.printf("success ");
		stdoutput.flush();
	} else {
		stdoutput.printf("\"%lld\"!=\"%lld\"\n",value,success);
		stdoutput.printf("failure: %s",cur->errorMessage());
		stdoutput.flush();
		delete cur;
		delete con;
		process::exit(1);
	}
}



// define the fields for the table that we're going to export
struct field_t {
	const char	*name;
	const char	*type;
	const char	*dbtype;
	const char	*pattern;
	bool		quote;
};

field_t field[]={
	{"testsmallint","smallint","SMALLINT","%lld",false},
	{"testmediumint","mediumint","MEDIUMINT","%lld",false},
	{"testint","int","INT","%lld",false},
	{"testbigint","bigint","BIGINT","%lld",false},
	{"testfloat","float","FLOAT","%lld.1",false},
	{"testreal","real","REAL","%lld.1",false},
	{"testdecimal","decimal(5,1)","DECIMAL","%lld.1",false},
	{"testdate","date","DATE","%04lld-01-01",true},
	{"testtime","time","TIME","01:00:00",true},
	{"testdatetime","datetime","DATETIME","%04lld-01-01 01:00:00",true},
	{"testchar","char(40)",
#ifdef HAVE_MYSQL_STMT_PREPARE
			"STRING",
#else
			"VARSTRING",
#endif
			"char%lld\t\"<>",true},
	{"testvarchar","varchar(40)","VARSTRING","varchar%lld\t\"<>",true},
	{"testtext","text","BLOB","text%lld\t\"<>",true},
	{"testtinytext","tinytext","TINYBLOB","tinytext%lld\t\"<>",true},
	{"testmediumtext","mediumtext",
#ifdef HAVE_MYSQL_STMT_PREPARE
			"MEDIUMBLOB",
#else
			"LONGBLOB",
#endif
			"mediumtext%lld\t\"<>",true},
	{"testlongtext","longtext","LONGBLOB","longtext%lld\t\"<>",true},
	{"testblob","blob","BLOB","blob%lld\t\"<>",true},
	{"testtinyblob","tinyblob","TINYBLOB","tinyblob%lld\t\"<>",true},
	{"testmediumblob","mediumblob",
#ifdef HAVE_MYSQL_STMT_PREPARE
			"MEDIUMBLOB",
#else
			"LONGBLOB",
#endif
			"mediumblob%lld\t\"<>",true,},
	{"testlongblob","longblob","LONGBLOB","longblob%lld\t\"<>",true},
	{NULL,NULL,NULL,NULL,false}
};

bool modifyField(const char *columnname,
			const char * const *columnstomodify,
			const char *columntype) {
	// only modify a field if it's in the set of columns to modify,
	// and it's type is not numeric or date/time
	return charstring::isInSet(columnname,columnstomodify) &&
					!isNumberTypeChar(columntype) &&
					!isDateTimeTypeChar(columntype);
}



// define a child of sqlrexport that overrides the various event methods,
// runs a set of tests in each one, and bails if any of the tests fail
class testsqlrexport : virtual public sqlrexport {
	public:
		testsqlrexport();

		void	setColumnsToModify(const char * const *columnstomodify);
		void	setRowsToExclude(dynamicarray<bool> *rowstoexclude);

		bool	exportStart();
		bool	columnsStart();
		bool	columnStart();
		bool	columnEnd();
		bool	columnsEnd();
		bool	rowsStart();
		bool	rowStart();
		bool	fieldStart();
		bool	fieldEnd();
		bool	rowEnd();
		bool	rowsEnd();
		bool	exportEnd();
		bool	error(int64_t errornumber, const char *errormessage);

		virtual bool	tests(const char *method);

	private:
		bool		testexcluderow;
		uint32_t	testcurrentcol;
		uint64_t	testcurrentrow;
		uint64_t	testexportedrowcount;
		bool		inrows;

		stringbuffer			modifiedcolumnname;
		linkedlist<stringbuffer *>	modifiedfields;

		const char * const	*columnstomodify;
		dynamicarray<bool>	*rowstoexclude;
};

testsqlrexport::testsqlrexport() : sqlrexport() {
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentrow=0;
	testexportedrowcount=0;
	inrows=false;
	columnstomodify=NULL;
	rowstoexclude=NULL;
	modifiedfields.setManageValues(true);
}

void testsqlrexport::setColumnsToModify(const char * const *columnstomodify) {
	this->columnstomodify=columnstomodify;
}

void testsqlrexport::setRowsToExclude(dynamicarray<bool> *rowstoexclude) {
	this->rowstoexclude=rowstoexclude;
}

bool testsqlrexport::tests(const char *method) {

	// test exclude row
	if (getExcludeRow()!=testexcluderow) {
		stdoutput.printf("\n%s - getExcludeRow(): %d!=%d\n",
					method,getExcludeRow(),testexcluderow);
		return false;
	}

	// test current row
	if (getCurrentRow()!=testcurrentrow) {
		stdoutput.printf("\n%s - getCurrentRow(): %lld!=%lld\n",
					method,getCurrentRow(),testcurrentrow);
		return false;
	}

	// test current column
	if (getCurrentColumn()!=testcurrentcol) {
		stdoutput.printf("\n%s - getCurrentColumn(): %ld!=%ld\n",
				method,getCurrentColumn(),testcurrentcol);
		return false;
	}

	// FIXME: do these like testsqlrimport::tests()...

	// test current column name (modifying it if necessary)
	const char	*colname=getSqlrCursor()->getColumnName(
							getCurrentColumn());

	stringbuffer	colnamestr;
	if (!charstring::compare(method,"columnEnd()") &&
			charstring::isInSet(colname,columnstomodify)) {
		colnamestr.append(colname)->append("MODIFIED");
		colname=colnamestr.getString();
	}

	if (charstring::compare(getCurrentColumnName(),colname)) {
		stdoutput.printf("\n%s - getCurrentColumnName(): %s!=%s\n",
					method,getCurrentColumnName(),colname);
		return false;
	}

	// test current field (modifying it if necessary)
	if (inrows && field[getCurrentColumn()].name!=NULL) {

		stringbuffer	f;
		f.printf(field[getCurrentColumn()].pattern,testcurrentrow);

		if (!charstring::compare(method,"fieldEnd()") &&
			modifyField(getCurrentColumnName(),
					columnstomodify,
					getSqlrCursor()->
						getColumnType(
							getCurrentColumn()))) {
			f.append("MODIFIED");
		}

		if (charstring::compare(getCurrentField(),f.getString())) {
			stdoutput.printf("\n%s - getCurrentField(): %s!=%s\n",
					method,getCurrentField(),
					f.getString());
			return false;
		}

	} else {

		if (charstring::compare(getCurrentField(),
					getCurrentColumnName())) {
			stdoutput.printf("\n%s - getCurrentField(): %s!=%s\n",
					method,getCurrentField(),
					getCurrentColumnName());
			return false;
		}
	}

	// test numeric column
	if (getIsNumericColumn(getCurrentColumn())!=
			isNumberTypeChar(getSqlrCursor()->
						getColumnType(
							getCurrentColumn()))) {
		stdoutput.printf("\n%s - getIsNumericColumn(%d): %d!=%d\n",
				method,getCurrentColumn(),
				getIsNumericColumn(getCurrentColumn()),
				isNumberTypeChar(
					getSqlrCursor()->
						getColumnType(
							getCurrentColumn())));
		return false;
	}

	// test exported row count
	if (getExportedRowCount()!=testexportedrowcount) {
		stdoutput.printf("\n%s - getExportedRowCount(): %lld!=%lld\n",
			method,getExportedRowCount(),testexportedrowcount);
		return false;
	}

	return true;
}

bool testsqlrexport::exportStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("\nexportStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentrow=0;
	testexportedrowcount=0;
	inrows=false;

	// call parent method
	if (!sqlrexport::exportStart()) {
		return false;
	}

	// run tests
	if (!tests("exportStart()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::columnsStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("\ncolumnsStart()...\n");
	#endif

	// call parent method
	if (!sqlrexport::columnsStart()) {
		return false;
	}

	// run tests
	if (!tests("columnsStart()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::columnStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("columnStart()...\n");
	#endif

	// call parent method
	if (!sqlrexport::columnStart()) {
		return false;
	}

	// run tests
	if (!tests("columnStart()")) {
		return false;
	}

	// modify column name, if necessary
	if (charstring::isInSet(getCurrentColumnName(),columnstomodify)) {
		modifiedcolumnname.clear();
		modifiedcolumnname.append(getCurrentColumnName());
		modifiedcolumnname.append("MODIFIED");
		setCurrentColumnName(modifiedcolumnname.getString());
	}
	return true;
}

bool testsqlrexport::columnEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("columnEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::columnEnd()) {
		return false;
	}

	// run tests
	if (!tests("columnEnd()")) {
		return false;
	}

	// increment current column
	testcurrentcol++;

	return true;
}

bool testsqlrexport::columnsEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("columnsEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::columnsEnd()) {
		return false;
	}

	// run tests
	if (!tests("columnsEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::rowsStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("rowsStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentrow=0;
	testexportedrowcount=0;
	inrows=true;

	// call parent method
	if (!sqlrexport::rowsStart()) {
		return false;
	}

	// run tests
	if (!tests("rowsStart()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::rowStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("rowStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	modifiedfields.clear();

	// call parent method
	if (!sqlrexport::rowStart()) {
		return false;
	}

	// exclude row, if necessary
	if (rowstoexclude && (*rowstoexclude)[testcurrentrow]) {
		setExcludeRow(true);
		testexcluderow=true;
	}

	// run tests
	if (!tests("rowStart()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::fieldStart() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("fieldStart()...\n");
	#endif

	// call parent method
	if (!sqlrexport::fieldStart()) {
		return false;
	}

	// run tests
	if (!tests("fieldStart()")) {
		return false;
	}

	// modify field, if necessary
	if (modifyField(getCurrentColumnName(),columnstomodify,
			getSqlrCursor()->getColumnType(getCurrentColumn()))) {
		stringbuffer	*modifiedfield=new stringbuffer();
		modifiedfield->append(getCurrentField());
		modifiedfield->append("MODIFIED");
		modifiedfields.append(modifiedfield);
		setCurrentField(modifiedfield->getString());
	}
	return true;
}

bool testsqlrexport::fieldEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("fieldEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::fieldEnd()) {
		return false;
	}

	// run tests
	if (!tests("fieldEnd()")) {
		return false;
	}

	// increment current column
	testcurrentcol++;

	return true;
}

bool testsqlrexport::rowEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("rowEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::rowEnd()) {
		return false;
	}

	// run tests
	if (!tests("rowEnd()")) {
		return false;
	}

	// increment row counts
	if (!testexcluderow) {
		testexportedrowcount++;
	}
	testcurrentrow++;

	return true;
}

bool testsqlrexport::rowsEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("rowsEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::rowsEnd()) {
		return false;
	}

	// run tests
	if (!tests("rowsEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::exportEnd() {

	#ifdef DEBUG_EXPORT
		stdoutput.printf("exportEnd()...\n");
	#endif

	// call parent method
	if (!sqlrexport::exportEnd()) {
		return false;
	}

	// run tests
	if (!tests("exportEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrexport::error(int64_t errornumber, const char *errormessage) {

	// print out error and bail
	stdoutput.printf("\n%lld: %s\n",errornumber,errormessage);
	return false;
}



// define a set of classes that inherit from our base class, to get the event
// methods, but also inherit from classes that export to csv, xml, and table,
// to get the export methods
class testsqlrexportfile : virtual public testsqlrexport,
				virtual public sqlrexportfile {
	public:
		testsqlrexportfile();
		void	setTestFileName(const char *testfilename);
		bool	tests(const char *method);
	private:
		const char	*testfilename;
};

testsqlrexportfile::testsqlrexportfile() : testsqlrexport() {
	testfilename=NULL;
}

void testsqlrexportfile::setTestFileName(const char *testfilename) {
	this->testfilename=testfilename;
}

bool testsqlrexportfile::tests(const char *method) {

	// call parent method
	if (!testsqlrexport::tests(method)) {
		return false;
	}

	// test file name
	if (charstring::compare(getFileName(),testfilename)) {
		stdoutput.printf("\n%s - getFileName(): %s!=%s\n",
				method,getFileName(),testfilename);
		return false;
	}
	return true;
}

class testsqlrexportcsv : virtual public testsqlrexportfile,
					virtual public sqlrexportcsv {
};

class testsqlrexportxml : virtual public testsqlrexportfile,
					virtual public sqlrexportxml {
};

class testsqlrexporttable : virtual public testsqlrexport,
					virtual public sqlrexporttable {
	public:
		testsqlrexporttable();
		void	setTestTable(const char *testtable);
		bool	tests(const char *method);
	private:
		const char	*testtable;
};

testsqlrexporttable::testsqlrexporttable() {
	testtable=NULL;
}

void testsqlrexporttable::setTestTable(const char *testtable) {
	this->testtable=testtable;
}

bool testsqlrexporttable::tests(const char *method) {

	// call parent method
	if (!testsqlrexport::tests(method)) {
		return false;
	}

	// test table name
	if (charstring::compare(getTable(),testtable)) {
		stdoutput.printf("\n%s - getTable(): %s!=%s\n",
				method,getTable(),testtable);
		return false;
	}
	return true;
}



// define a child of sqlrimport that overrides the various event methods,
// runs a set of tests in each one, and bails if any of the tests fail
class testsqlrimport : virtual public sqlrimport {
	public:
		testsqlrimport();

		void	setColumnsToModify(const char * const *columnstomodify);
		void	setRowsToIgnore(dynamicarray<bool> *rowstoignore);
		void	setEmptyRows(dynamicarray<bool> *emptyrows);

		bool	importStart();
		bool	columnsStart();
		bool	columnStart();
		bool	columnEnd();
		bool	columnsEnd();
		bool	rowsStart();
		bool	rowStart();
		bool	fieldStart();
		bool	fieldEnd();
		bool	rowEnd();
		bool	rowsEnd();
		bool	importEnd();
		bool	error(int64_t errornumber, const char *errormessage);

		virtual bool	tests(const char *method);

	private:
		bool		testexcluderow;
		uint32_t	testcurrentcol;
		const char	*testcurrentcolname;
		stringbuffer	testcurrentcolnamestr;
		uint64_t	testcurrentrow;
		const char	*testcurrentfield;
		stringbuffer	testcurrentfieldstr;
		dynamicarray<bool>	testisnumeric;
		dynamicarray<bool>	testisdatetime;
		uint64_t	testimportedrowcount;

		const char * const	*columnstomodify;
		dynamicarray<bool>	*rowstoignore;
		dynamicarray<bool>	*emptyrows;
};

testsqlrimport::testsqlrimport() : sqlrimport() {
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentcolname=NULL;
	testcurrentrow=0;
	testcurrentfield=NULL;
	for (uint64_t col=0; field[col].name; col++) {
		testisnumeric[col]=false;
		testisdatetime[col]=false;
	}
	testimportedrowcount=0;
	columnstomodify=NULL;
	rowstoignore=NULL;
	emptyrows=NULL;
}

void testsqlrimport::setColumnsToModify(const char * const *columnstomodify) {
	this->columnstomodify=columnstomodify;
}

void testsqlrimport::setRowsToIgnore(dynamicarray<bool> *rowstoignore) {
	this->rowstoignore=rowstoignore;
}

void testsqlrimport::setEmptyRows(dynamicarray<bool> *emptyrows) {
	this->emptyrows=emptyrows;
}

bool testsqlrimport::tests(const char *method) {

	// test exclude row
	if (getExcludeRow()!=testexcluderow) {
		stdoutput.printf("\n%s - getExcludeRow(): %d!=%d\n",
					method,getExcludeRow(),testexcluderow);
		return false;
	}

	// test current row
	if (getCurrentRow()!=testcurrentrow) {
		stdoutput.printf("\n%s - getCurrentRow(): %lld!=%lld\n",
					method,getCurrentRow(),testcurrentrow);
		return false;
	}

	// test current column
	if (getCurrentColumn()!=testcurrentcol) {
		stdoutput.printf("\n%s - getCurrentColumn(): %ld!=%ld\n",
				method,getCurrentColumn(),testcurrentcol);
		return false;
	}

	// test current column name
	if (charstring::compare(getCurrentColumnName(),testcurrentcolname)) {
		stdoutput.printf(
			"\n%s - getCurrentColumnName(): %s!=%s\n",
			method,getCurrentColumnName(),testcurrentcolname);
		return false;
	}

	// test current field
	if (charstring::compare(getCurrentField(),testcurrentfield)) {
		stdoutput.printf(
			"\n%s - getCurrentField(): %s!=%s\n",
			method,getCurrentField(),testcurrentfield);
		return false;
	}

	// test numeric column
	if (getCurrentColumn()<testisnumeric.getCount() &&
			getIsNumericColumn(getCurrentColumn())!=
					testisnumeric[getCurrentColumn()]) {
		stdoutput.printf("\n%s - getIsNumericColumn(%d): %d!=%d\n",
			method,getCurrentColumn(),
			getIsNumericColumn(getCurrentColumn()),
			testisnumeric[getCurrentColumn()]);
		return false;
	}

	// test datetime column
	if (getCurrentColumn()<testisdatetime.getCount() &&
			getIsDateTimeColumn(getCurrentColumn())!=
					testisdatetime[getCurrentColumn()]) {
		stdoutput.printf("\n%s - getIsDateTimeColumn(%d): %d!=%d\n",
			method,getCurrentColumn(),
			getIsDateTimeColumn(getCurrentColumn()),
			testisdatetime[getCurrentColumn()]);
		return false;
	}

	// test imported row count
	if (getImportedRowCount()!=testimportedrowcount) {
		stdoutput.printf("\n%s - getImportedRowCount(): %lld!=%lld\n",
			method,getImportedRowCount(),testimportedrowcount);
		return false;
	}

	return true;
}

bool testsqlrimport::importStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nimportStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentcolname=NULL;
	testcurrentrow=0;
	testcurrentfield=NULL;
	for (uint64_t col=0; field[col].name; col++) {
		testisnumeric[col]=false;
		testisdatetime[col]=false;
	}
	testimportedrowcount=0;

	// call parent method
	if (!sqlrimport::importStart()) {
		return false;
	}

	// run tests
	if (!tests("importStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::columnsStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\ncolumnsStart()...\n");
	#endif

	// call parent method
	if (!sqlrimport::columnsStart()) {
		return false;
	}

	// run tests
	if (!tests("columnsStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::columnStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\ncolumnStart()...\n");
	#endif

	// set column name to test against (modifying it if necessary)
	testcurrentcolname=field[getCurrentColumn()].name;
	if (charstring::isInSet(testcurrentcolname,columnstomodify)) {
		testcurrentcolnamestr.clear();
		testcurrentcolnamestr.append(testcurrentcolname);
		testcurrentcolnamestr.append("MODIFIED");
		testcurrentcolname=testcurrentcolnamestr.getString();
	}
	testcurrentfield=testcurrentcolname;

	// call parent method
	if (!sqlrimport::columnStart()) {
		return false;
	}

	// run tests
	if (!tests("columnStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::columnEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\ncolumnEnd()...\n");
	#endif

	// call parent method
	if (!sqlrimport::columnEnd()) {
		return false;
	}

	// run tests
	if (!tests("columnEnd()")) {
		return false;
	}

	// increment current column
	testcurrentcol++;

	return true;
}

bool testsqlrimport::columnsEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\ncolumnsEnd()...\n");
	#endif

	// reset flags and counts
	testcurrentcolname=NULL;
	testcurrentfield=NULL;

	// set isnumeric/isdatetime flags
	for (uint64_t col=0; field[col].name; col++) {
		testisnumeric[col]=isNumberTypeChar(field[col].dbtype);
		testisdatetime[col]=isDateTimeTypeChar(field[col].dbtype);
	}

	// call parent method
	if (!sqlrimport::columnsEnd()) {
		return false;
	}

	// run tests
	if (!tests("columnsEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::rowsStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nrowsStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentrow=0;
	testimportedrowcount=0;

	// call parent method
	if (!sqlrimport::rowsStart()) {
		return false;
	}

	// run tests
	if (!tests("rowsStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::rowStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nrowStart()...\n");
	#endif

	// reset flags and counts
	testexcluderow=false;
	testcurrentcol=0;
	testcurrentcolname=NULL;
	testcurrentfield=NULL;

	// call parent method
	if (!sqlrimport::rowStart()) {
		return false;
	}

	// run tests
	if (!tests("rowStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::fieldStart() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nfieldStart()...\n");
	#endif

	// set column name to test against
	testcurrentcolname=field[getCurrentColumn()].name;

	// set field to test against (modifying it if necessary)
	testcurrentfieldstr.clear();
	if (emptyrows && (*emptyrows)[getCurrentRow()]) {
		testcurrentfield=NULL;
	} else {
		testcurrentfieldstr.printf(
			field[getCurrentColumn()].pattern,getCurrentRow());
		if (modifyField(getCurrentColumnName(),columnstomodify,
					field[getCurrentColumn()].dbtype)) {
			testcurrentfieldstr.append("MODIFIED");
		}
		testcurrentfield=testcurrentfieldstr.getString();
	}

	// call parent method
	if (!sqlrimport::fieldStart()) {
		return false;
	}

	// run tests
	if (!tests("fieldStart()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::fieldEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nfieldEnd()...\n");
	#endif

	// call parent method
	if (!sqlrimport::fieldEnd()) {
		return false;
	}

	// run tests
	if (!tests("fieldEnd()")) {
		return false;
	}

	// increment current column
	testcurrentcol++;

	return true;
}

bool testsqlrimport::rowEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nrowEnd()...\n");
	#endif

	// reset flags and counts
	testcurrentcolname=NULL;
	testcurrentfield=NULL;

	// call parent method
	if (!sqlrimport::rowEnd()) {
		return false;
	}

	// run tests
	if (!tests("rowEnd()")) {
		return false;
	}

	// increment row counts
	if (!testexcluderow && !(emptyrows && (*emptyrows)[getCurrentRow()])) {
		testimportedrowcount++;
	}
	testcurrentrow++;

	return true;
}

bool testsqlrimport::rowsEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nrowsEnd()...\n");
	#endif

	// call parent method
	if (!sqlrimport::rowsEnd()) {
		return false;
	}

	// run tests
	if (!tests("rowsEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::importEnd() {

	#ifdef DEBUG_IMPORT
		stdoutput.printf("\nimportEnd()...\n");
	#endif

	// call parent method
	if (!sqlrimport::importEnd()) {
		return false;
	}

	// run tests
	if (!tests("importEnd()")) {
		return false;
	}
	return true;
}

bool testsqlrimport::error(int64_t errornumber, const char *errormessage) {

	// print out error and bail
	stdoutput.printf("\n%lld: %s\n",errornumber,errormessage);
	return false;
}




// define a set of classes that inherit from our base class, to get the event
// methods, but also inherit from classes that import to csv, xml, and table,
// to get the import methods
class testsqlrimportfile : virtual public testsqlrimport,
				virtual public sqlrimportfile {
	public:
		testsqlrimportfile();
		void	setTestFileName(const char *testfilename);
		bool	tests(const char *method);
	private:
		const char	*testfilename;
};

testsqlrimportfile::testsqlrimportfile() : testsqlrimport() {
	testfilename=NULL;
}

void testsqlrimportfile::setTestFileName(const char *testfilename) {
	this->testfilename=testfilename;
}

bool testsqlrimportfile::tests(const char *method) {

	// call parent method
	if (!testsqlrimport::tests(method)) {
		return false;
	}

	// test file name
	if (charstring::compare(getFileName(),testfilename)) {
		stdoutput.printf("\n%s - getFileName(): %s!=%s\n",
				method,getFileName(),testfilename);
		return false;
	}
	return true;
}

class testsqlrimportcsv : virtual public testsqlrimportfile,
					virtual public sqlrimportcsv {
};

class testsqlrimportxml : virtual public testsqlrimportfile,
					virtual public sqlrimportxml {
};



// define a set of methods that generate something to compare our export to
void generateCsv(const char *option,
			const char *filename,
			bool excludecolumns,
			const char * const *columnstoexclude,
			const char * const *columnstomodify,
			dynamicarray<bool> *rowstoexclude,
			dynamicarray<bool> *emptyrows) {

	stdoutput.printf("%sGENERATE CSV:\n",option);

	// create file
	file	comparison;
	checkSuccess(comparison.create(filename,
				permissions::parsePermString("rw-rw-r--")),1);
	comparison.setWriteBufferSize(
		filesystem::getOptimumTransferBlockSize(filename));

	// write header, ignoring and modifying columns as necessary
	stringbuffer	header;
	for (uint64_t col=0; field[col].name; col++) {
		if (charstring::isInSet(field[col].name,columnstoexclude)) {
			continue;
		}
		if (header.getSize()) {
			header.append(',');
		}
		header.append('"');
		header.append(field[col].name);
		if (charstring::isInSet(field[col].name,columnstomodify)) {
			header.append("MODIFIED");
		}
		header.append('"');
	}
	if (!excludecolumns) {
		header.append('\n');
		checkSuccess((int)comparison.write(header.getString()),
					(int)header.getStringLength());
	}

	// write records, ignoring columns and modifying fields as necessary
	stringbuffer	record;
	for (uint64_t row=0; row<ROWS; row++) {
		if (rowstoexclude && (*rowstoexclude)[row]) {
			continue;
		}
		bool	first=true;
		for (uint32_t col=0; field[col].name; col++) {
			if (charstring::isInSet(field[col].name,
							columnstoexclude)) {
				continue;
			}
			if (!first) {
				record.append(',');
			}
			if (!emptyrows || !(*emptyrows)[row]) {
				if (field[col].quote) {
					record.append('"');
				}
				char	*fld;
				charstring::printf(
					&fld,field[col].pattern,row);
				for (const char *f=fld; *f; f++) {
					if (*f=='"') {
						record.append("\"\"");
					} else {
						record.append(*f);
					}
				}
				delete[] fld;
				if (modifyField(field[col].name,
					columnstomodify,field[col].dbtype)) {
					record.append("MODIFIED");
				}
				if (field[col].quote) {
					record.append('"');
				}
			}
			first=false;
		}
		record.append('\n');
		if (comparison.write(record.getString(),
					record.getStringLength())!=
					(ssize_t)record.getStringLength()) {
			checkSuccess(0,1);
		}
		record.clear();
	}
	checkSuccess(1,1);

	comparison.flushWriteBuffer(-1,-1);

	stdoutput.printf("\n\n");
}

void generateXml(const char *option,
			const char *filename,
			uint32_t colcount, bool excludecolumns,
			const char * const *columnstoexclude,
			const char * const *columnstomodify,
			dynamicarray<bool> *rowstoexclude,
			dynamicarray<bool> *emptyrows) {

	stdoutput.printf("%sGENERATE XML:\n",option);

	// create file
	file	comparison;
	checkSuccess(comparison.create(filename,
				permissions::parsePermString("rw-rw-r--")),1);
	comparison.setWriteBufferSize(
		filesystem::getOptimumTransferBlockSize(filename));

	// write header, ignoring and modifying columns as necessary
	comparison.write("<?xml version=\"1.0\"?>\n");
	comparison.write("<table>\n");
	if (!excludecolumns) {
		stringbuffer	header;
		header.printf("<columns count=\"%ld\">\n",colcount);
		for (uint64_t col=0; field[col].name; col++) {
			if (charstring::isInSet(field[col].name,
							columnstoexclude)) {
				continue;
			}
			const char	*colname=field[col].name;
			const char	*colnameext="";
			if (charstring::isInSet(field[col].name,
							columnstomodify)) {
				colnameext="MODIFIED";
			}
			header.printf("	<column name=\"%s%s\" "
						"type=\"%s\"/>\n",
						colname,colnameext,
						field[col].dbtype);
		}
		header.append("</columns>\n");
		checkSuccess((int)comparison.write(header.getString()),
					(int)header.getStringLength());
	}

	// write records, ignoring columns and modifying fields as necessary
	stringbuffer	record;
	comparison.write("<rows>\n");
	for (uint64_t row=0; row<ROWS; row++) {
		if (rowstoexclude && (*rowstoexclude)[row]) {
			continue;
		}
		record.append("	<row>\n");
		for (uint32_t col=0; field[col].name; col++) {
			if (charstring::isInSet(field[col].name,
							columnstoexclude)) {
				continue;
			}
			record.append("	<field>");
			if (!emptyrows || !(*emptyrows)[row]) {
				char	*fld;
				charstring::printf(&fld,field[col].pattern,row);
				for (const char *f=fld; *f; f++) {
					if (*f=='"' || *f<' ' || *f>'~' ||
						*f=='&' || *f=='<' || *f=='>') {
						record.printf(
							"&%d;",(uint8_t)*f);
					} else {
						record.write(*f);
					}
				}
				delete[] fld;
				if (modifyField(field[col].name,
					columnstomodify,field[col].dbtype)) {
					record.append("MODIFIED");
				}
			}
			record.append("</field>\n");
		}
		record.append("	</row>\n");
		if (comparison.write(record.getString(),
					record.getStringLength())!=
					(ssize_t)record.getStringLength()) {
			checkSuccess(0,1);
		}
		record.clear();
	}
	comparison.write("</rows>\n");
	comparison.write("</table>\n");
	checkSuccess(1,1);

	comparison.flushWriteBuffer(-1,-1);

	stdoutput.printf("\n\n");
}

void createTable(const char *tablename,
			const char * const *columnstoexclude,
			const char * const *columnstomodify,
			uint32_t *colcount) {

	// create a table as defined by field[], ignoring columns as necessary
	stringbuffer	query;
	query.append("create table ")->append(tablename)->append(" (");
	uint32_t	ccount=0;
	for (uint64_t col=0; field[col].name; col++) {
		if (charstring::isInSet(field[col].name,columnstoexclude)) {
			continue;
		}
		if (ccount) {
			query.append(',');
		}
		query.append(field[col].name);
		if (charstring::isInSet(field[col].name,columnstomodify)) {
			query.append("MODIFIED");
		}
		query.append(' ');
		query.append(field[col].type);
		ccount++;
	}
	query.append(')');
	cur->sendQuery(query.getString());
	if (colcount) {
		*colcount=ccount;
	}
}

void generateTable(const char *option,
			const char *tablename,
			const char * const *columnstoexclude,
			const char * const *columnstomodify,
			dynamicarray<bool> *rowstoexclude,
			dynamicarray<bool> *emptyrows) {

	stdoutput.printf("%sGENERATE TABLE:\n",option);

	// connect to db
	sqlrconnection	econ("sqlrelay",9000,"/tmp/test.socket",
					"testuser","testpassword",0,1);
	sqlrcursor	ecur(&econ);

	// create table
	createTable(tablename,columnstoexclude,columnstomodify,NULL);

	// populate table, ignoring and modifying fields as necessary
	bool		success=true;
	stringbuffer	query;
	for (uint64_t row=0; row<ROWS; row++) {
		if (rowstoexclude && (*rowstoexclude)[row]) {
			continue;
		}
		if (emptyrows && (*emptyrows)[row]) {
			continue;
		}
		query.clear();
		query.append("insert into ");
		query.append(tablename);
		query.append(" values (");
		bool	first=true;
		for (uint32_t col=0; field[col].pattern; col++) {
			if (charstring::isInSet(field[col].name,
							columnstoexclude)) {
				continue;
			}
			if (first) {
				first=false;
			} else {
				query.append(',');
			}
			if (field[col].quote) {
				query.append('\'');
			}
			query.printf(field[col].pattern,row);
			if (charstring::isInSet(field[col].name,
							columnstomodify) &&
				!isNumberTypeChar(field[col].dbtype) &&
				!isDateTimeTypeChar(field[col].dbtype)) {
				query.append("MODIFIED");
			}
			if (field[col].quote) {
				query.append('\'');
			}
		}
		query.append(')');
		if (!ecur.sendQuery(query.getString())) {
			success=false;
			break;
		}
	}

	checkSuccess(success,1);

	econ.commit();

	stdoutput.printf("\n\n");
}


// define methods to diff our export against a comparison
void diffFiles(const char *filename1, const char *filename2) {

	// open file 1
	file	f1;
	checkSuccess(f1.open(filename1,O_RDONLY),1);
	f1.setReadBufferSize(
		filesystem::getOptimumTransferBlockSize(filename1));

	// open file 2
	file	f2;
	checkSuccess(f2.open(filename2,O_RDONLY),1);
	f2.setReadBufferSize(
		filesystem::getOptimumTransferBlockSize(filename2));

	for (;;) {

		// lines
		char	*line1;
		char	*line2;

		// read a line from each file
		ssize_t	size1=f1.read(&line1,"\n");
		ssize_t	size2=f2.read(&line2,"\n");

		// fail if the sizes are different
		if (size1!=size2) {
			delete[] line1;
			delete[] line2;
			checkSuccess(0,1);
		}

		// fail if the lines are not the same
		if (charstring::compare(line1,line2)) {
			delete[] line1;
			delete[] line2;
			checkSuccess(0,1);
		}

		// bail if we failed to read either line
		if (!size1 || !size2) {
			delete[] line1;
			delete[] line2;
			break;
		}
	}
	checkSuccess(1,1);
}

void diffTables(const char *table1, const char *table2) {
	
	// select from table 1
	sqlrconnection	con1("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
	sqlrcursor	cur1(&con1);
	stringbuffer	q1;
	q1.append("select * from ");
	q1.append(table1);
	q1.append(" order by testsmallint");
	cur1.setResultSetBufferSize(10);
	cur1.sendQuery(q1.getString());

	// select from table 2
	sqlrconnection	con2("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
	sqlrcursor	cur2(&con2);
	stringbuffer	q2;
	q2.append("select * from ");
	q2.append(table2);
	q2.append(" order by testsmallint");
	cur2.setResultSetBufferSize(10);
	cur2.sendQuery(q2.getString());

	// fail if the column counts are different
	checkSuccess((int)cur1.colCount(),(int)cur2.colCount());

	// run through the rows...
	uint64_t	row=0;
	for (;;) {

		// determine if we're at the end of either result set
		bool	rowend1=(cur1.endOfResultSet() && row>cur1.rowCount());
		bool	rowend2=(cur2.endOfResultSet() && row>cur2.rowCount());

		// fail if the row counts are different
		if (rowend1 && !rowend2) {
			checkSuccess(0,1);
		}
		if (rowend2 && !rowend1) {
			checkSuccess(0,1);
		}

		// bail if we're at the end of both result sets
		if (rowend1 && rowend2) {
			break;
		}

		// run through the fields...
		for (uint32_t col=0; col<cur1.colCount(); col++) {

			// bail if the fields aren't the same
			if (charstring::compare(cur1.getField(row,col),
						cur2.getField(row,col))) {
				checkSuccess(0,1);
			}
		}
		row++;
	}
}


void exportTests() {

	stdoutput.printf("EXPORT TESTS... \n\n");

	// clean up
	cur->sendQuery("drop table testtable");
	cur->sendQuery("drop table testtable_export");
	cur->sendQuery("drop table testtable_comparison");
	file::remove("testtable.csv");
	file::remove("testtable-comparison.csv");
	file::remove("testtable.xml");
	file::remove("testtable-comparison.xml");

	// create a new table
	stdoutput.printf("CREATE TESTTABLE: \n");
	uint32_t	colcount=0;
	createTable("testtable",NULL,NULL,&colcount);
	stdoutput.printf("\n");

	// insert
	stdoutput.printf("INSERT: \n");
	stringbuffer	query;
	for (uint64_t row=0; row<ROWS; row++) {
		query.clear();
		query.append("insert into testtable values (");
		for (uint32_t col=0; field[col].pattern; col++) {
			if (col) {
				query.append(',');
			}
			if (field[col].quote) {
				query.append('\'');
			}
			query.printf(field[col].pattern,row);
			if (field[col].quote) {
				query.append('\'');
			}
		}
		query.append(')');
		if (!cur->sendQuery(query.getString())) {
			checkSuccess(0,1);
		}
	}
	checkSuccess(1,1);
	stdoutput.printf("\n\n");

	// set up csv export
	stdoutput.printf("SET UP CSV EXPORT: \n");
	testsqlrexportcsv	tsec;
	tsec.setSqlrConnection(con);
	tsec.setSqlrCursor(cur);
	checkSuccess((uint64_t)tsec.getSqlrConnection(),(uint64_t)con);
	checkSuccess((uint64_t)tsec.getSqlrCursor(),(uint64_t)cur);
	stdoutput.printf("\n\n");

	// set up xml export
	stdoutput.printf("SET UP XML EXPORT: \n");
	testsqlrexportxml	tsex;
	tsex.setSqlrConnection(con);
	tsex.setSqlrCursor(cur);
	checkSuccess((uint64_t)tsex.getSqlrConnection(),(uint64_t)con);
	checkSuccess((uint64_t)tsex.getSqlrCursor(),(uint64_t)cur);
	stdoutput.printf("\n\n");

	// set up table export
	stdoutput.printf("SET UP TABLE EXPORT: \n");
	testsqlrexporttable	tset;
	tset.setSqlrConnection(con);
	tset.setSqlrCursor(cur);
	checkSuccess((uint64_t)tset.getSqlrConnection(),(uint64_t)con);
	checkSuccess((uint64_t)tset.getSqlrCursor(),(uint64_t)cur);
	stdoutput.printf("\n\n");

	// iterate through options...
	uint8_t oiter=0;
	for (;;) {

		// set options for ignoring/modifying rows/columns/fields
		const char		*option=NULL;
		bool			excludecolumns=false;
		const char		**columnstoexclude=NULL;
		uint16_t		columnstoexcludecount=0;
		const char		**columnstomodify=NULL;
		dynamicarray<bool>	rowstoexclude;
		for (uint64_t row=0; row<ROWS; row++) {
			rowstoexclude[row]=false;
		}
		stringbuffer		opt;
		const char		*col;
		if (oiter==0) {
			option=charstring::duplicate("");
		} else if (oiter==1) {
			// for iteration 1, exclude columns
			option=charstring::duplicate("IGNORE COLUMNS - ");
			excludecolumns=true;
		} else if (oiter>=2 && oiter<=21) {
			// for iterations 2-21, exclude individual columns
			col=field[oiter-2].name;
			columnstoexclude=new const char *[2];
			columnstoexclude[0]=col;
			columnstoexclude[1]=NULL;
			columnstoexcludecount=1;
			opt.append("IGNORE ");
			opt.append(col);
			opt.append(" column - ");
			option=opt.detachString();
		} else if (oiter>=22 && oiter<=32) {
			// for iterations 22-32, exclude random sets of
			// columns, possibly with repetitions
			randomnumber	r;
			r.setSeed(randomnumber::getSeed());
			opt.append("IGNORE ");
			columnstoexclude=new const char *[11];
			uint32_t	rn;
			for (uint8_t i=0; i<10; i++) {
				r.generate(&rn);
				r.setSeed(rn);
				rn=r.scale(rn,0,19);
				col=field[rn].name;
				columnstoexclude[i]=col;
				if (i) {
					opt.append(',');
				}
				opt.append(col);
			}
			columnstoexclude[10]=NULL;
			columnstoexcludecount=10;
			opt.append(" column - ");
			option=opt.detachString();
		} else if (oiter==33) {
			// for iteration 33, don't export various rows
			opt.append("IGNORE ");
			randomnumber	r;
			r.setSeed(randomnumber::getSeed());
			uint32_t	rn;
			uint64_t	ircount=0;
			for (uint64_t row=0; row<ROWS; row++) {
				r.generate(&rn);
				r.setSeed(rn);
				rn=r.scale(rn,0,1);
				if (rn) {
					rowstoexclude[row]=true;
					ircount++;
				}
			}
			opt.append(ircount);
			opt.append(" ROWS - ");
			option=opt.detachString();
		} else if (oiter>=34 && oiter<=44) {
			// for iterations 34-44, modify random sets of
			// columns and fields, possibly with repetitions
			randomnumber	r;
			r.setSeed(randomnumber::getSeed());
			opt.append("MODIFY ");
			columnstomodify=new const char *[11];
			uint32_t	rn;
			for (uint8_t i=0; i<10; i++) {
				r.generate(&rn);
				r.setSeed(rn);
				rn=r.scale(rn,0,19);
				col=field[rn].name;
				columnstomodify[i]=col;
				if (i) {
					opt.append(',');
				}
				opt.append(col);
			}
			columnstomodify[10]=NULL;
			opt.append(" column - ");
			option=opt.detachString();
		} else {
			break;
		}

		// iterate through formats...
		// (1==CSV, 2=XML, 3==TABLE)
		for (uint8_t fiter=0; fiter<3; fiter++) {

			// csv, xml, or table
			testsqlrexport	*e;
			const char	*format;
			const char	*exp;
			const char	*comp;
			if (fiter==0) {
				e=&tsec;
				format="CSV";
				exp="testtable.csv";
				comp="testtable-comparison.csv";
			} else if (fiter==1) {
				e=&tsex;
				format="XML";
				exp="testtable.xml";
				comp="testtable-comparison.xml";
			} else if (fiter==2) {
				e=&tset;
				format="TABLE";
				exp="testtable_export";
				comp="testtable_comparison";
				createTable(exp,columnstoexclude,
						columnstomodify,NULL);
			}

			// export file/table
			stdoutput.printf("%sEXPORT %s: \n",option,format);
			e->setExcludeColumns(excludecolumns);
			e->setColumnsToExclude(columnstoexclude);
			e->setColumnsToModify(columnstomodify);
			e->setRowsToExclude(&rowstoexclude);
			checkSuccess(e->getExcludeColumns(),excludecolumns);
			if (columnstoexclude) {
				for (uint16_t i=0;
					i<columnstoexcludecount; i++) {
					checkSuccess(
						e->getColumnsToExclude()[i],
						columnstoexclude[i]);
				}
				checkSuccess(
					e->getColumnsToExclude()[
						columnstoexcludecount],
					NULL);
			} else {
				checkSuccess(
					(uint64_t)e->getColumnsToExclude(),
					(uint64_t)0);
			}
			checkSuccess(cur->sendQuery(
				"select * from testtable "
				"order by testsmallint"),1);
			if (fiter==0) {
				tsec.setFileName(exp);
				tsec.setTestFileName(exp);
				checkSuccess(tsec.exportData(),1);
			} else if (fiter==1) {
				tsex.setFileName(exp);
				tsex.setTestFileName(exp);
				checkSuccess(tsex.exportData(),1);
			} else if (fiter==2) {
				sqlrconnection	econ("sqlrelay",9000,
							"/tmp/test.socket",
							"testuser",
							"testpassword",0,1);
				sqlrcursor	ecur(&econ);
				tset.setExportSqlrConnection(&econ);
				tset.setExportSqlrCursor(&ecur);
				tset.setTable(exp);
				tset.setCommitCount(500);
				tset.setTestTable(exp);
				checkSuccess(tset.exportData(),1);
				econ.commit();
			}
			stdoutput.printf("\n\n");

			// generate comparison file/table
			if (fiter==0) {
				generateCsv(option,comp,
					excludecolumns,columnstoexclude,
					columnstomodify,&rowstoexclude,
					NULL);
			} else if (fiter==1) {
				generateXml(option,comp,colcount,
					excludecolumns,columnstoexclude,
					columnstomodify,&rowstoexclude,
					NULL);
			} else if (fiter==2) {
				generateTable(option,comp,
					columnstoexclude,
					columnstomodify,&rowstoexclude,
					NULL);
			}

			// diff files/tables
			stdoutput.printf("%sDIFF %s: \n",option,format);
			if (fiter<2) {
				diffFiles(exp,comp);
			} else {
				diffTables(exp,comp);
			}
			stdoutput.printf("\n");

			// clean up
			if (fiter==2) {
				sqlrconnection	ccon("sqlrelay",9000,
							"/tmp/test.socket",
							"testuser",
							"testpassword",0,1);
				sqlrcursor	ccur(&ccon);

				ccur.prepareQuery("drop table $(EXP)");
				ccur.substitution("EXP",exp);
				ccur.executeQuery();
				
				ccur.prepareQuery("drop table $(COMP)");
				ccur.substitution("COMP",comp);
				ccur.executeQuery();
			}
			file::remove(exp);
			file::remove(comp);

			stdoutput.printf("\n");
		}

		// clean up
		delete[] option;
		delete[] columnstoexclude;

		oiter++;
	}

	// clean up
	cur->sendQuery("drop table testtable");

	stdoutput.printf("\n\n");
}


void importTests() {

	stdoutput.printf("IMPORT TESTS... \n\n");

	// clean up
	cur->sendQuery("drop table testtable");
	cur->sendQuery("drop table testtable_comparison");
	file::remove("testtable.csv");
	file::remove("testtable.xml");

	// create a new table
	stdoutput.printf("CREATE TESTTABLE: \n");
	uint32_t	colcount=0;
	createTable("testtable",NULL,NULL,&colcount);
	stdoutput.printf("\n");

	// set up csv import
	stdoutput.printf("SET UP CSV IMPORT: \n");
	testsqlrimportcsv	tsic;
	tsic.setSqlrConnection(con);
	tsic.setSqlrCursor(cur);
	tsic.setDbType(con->identify());
	checkSuccess((uint64_t)tsic.getSqlrConnection(),(uint64_t)con);
	checkSuccess((uint64_t)tsic.getSqlrCursor(),(uint64_t)cur);
	stdoutput.printf("\n\n");

	// set up xml import
	stdoutput.printf("SET UP XML IMPORT: \n");
	testsqlrimportxml	tsix;
	tsix.setSqlrConnection(con);
	tsix.setSqlrCursor(cur);
	tsix.setDbType(con->identify());
	checkSuccess((uint64_t)tsix.getSqlrConnection(),(uint64_t)con);
	checkSuccess((uint64_t)tsix.getSqlrCursor(),(uint64_t)cur);
	stdoutput.printf("\n\n");

	// iterate through options...
	uint8_t oiter=0;
	for (;;) {

		// set options for ignoring/modifying rows/columns/fields
		const char		*option=NULL;
		bool			ignorecolumns=false;
		dynamicarray<bool>	emptyrows;
		for (uint64_t i=0; i<ROWS; i++) {
			emptyrows[i]=false;
		}
		stringbuffer		opt;
		if (oiter==0) {
			option=charstring::duplicate("");
		} else if (oiter==1) {
			// for iteration 1, ignore columns
			option=charstring::duplicate("IGNORE COLUMNS - ");
			ignorecolumns=true;
		} else if (oiter>=2 && oiter<=12) {
			// for iterations 2-12, randomly replace
			// rows with empty rows, and ignore them
			opt.append("IGNORE EMPTY ROWS - ");
			randomnumber	r;
			r.setSeed(r.getSeed());
			uint64_t	emptycount=0;
			for (uint64_t i=0; i<ROWS; i++) {
				uint32_t	result;
				r.generate(&result);
				r.setSeed(result);
				bool	empty=r.scale(result,0,1);
				emptyrows[i]=empty;
				if (empty) {
					emptycount++;
				}
			}
			opt.append(emptycount)->append(" empty rows - ");
			option=opt.detachString();
		} else {
			// FIXME: test modifying fields during import
			// FIXME: test ignore columns with empty names
			// FIXME: test insert primary key
			// FIXME: test insert static values
			break;
		}

		// iterate through formats...
		// (1==CSV, 2=XML)
		for (uint8_t fiter=0; fiter<2; fiter++) {

			// csv or xml
			testsqlrimport	*im;
			const char	*format;
			const char	*imp;
			if (fiter==0) {
				im=&tsic;
				format="CSV";
				imp="testtable.csv";
				generateCsv(option,imp,
						false,NULL,NULL,NULL,
						&emptyrows);
				tsic.setFileName(imp);
				tsic.setTestFileName(imp);
			} else if (fiter==1) {
				im=&tsix;
				format="XML";
				imp="testtable.xml";
				generateXml(option,imp,colcount,
						false,NULL,NULL,NULL,
						&emptyrows);
				tsix.setFileName(imp);
				tsix.setTestFileName(imp);
			}

			// import file/table
			stdoutput.printf("%sIMPORT %s: \n",option,format);
			im->setIgnoreColumns(ignorecolumns);
			checkSuccess(im->getIgnoreColumns(),ignorecolumns);
			im->setEmptyRows(&emptyrows);
			im->setIgnoreEmptyRows(true);
			checkSuccess(im->importData(),1);
			stdoutput.printf("\n\n");

			// generate comparison table
			generateTable(option,"testtable_comparison",
							NULL,NULL,NULL,
							&emptyrows);

			// diff tables
			stdoutput.printf("%sDIFF TABLES: \n",option);
			diffTables("testtable","testtable_comparison");
			stdoutput.printf("\n");

			// clean up
			sqlrconnection	ccon("sqlrelay",9000,
						"/tmp/test.socket",
						"testuser",
						"testpassword",0,1);
			sqlrcursor	ccur(&ccon);
			ccur.sendQuery("delete from testtable");
			ccur.sendQuery("drop table testtable_comparison");
			file::remove(imp);

			stdoutput.printf("\n");
		}

		// clean up
		delete[] option;

		oiter++;
	}

	// clean up
	cur->sendQuery("drop table testtable");
}


int main(int argc, char **argv) {

	con=new sqlrconnection("sqlrelay",9000,"/tmp/test.socket",
						"testuser","testpassword",0,1);
	cur=new sqlrcursor(con);

	exportTests();
	importTests();

	delete cur;
	delete con;

	return 0;
}
