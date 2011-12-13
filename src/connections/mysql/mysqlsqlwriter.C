#include <mysqlsqlwriter.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslatordebug.h>

mysqlsqlwriter::mysqlsqlwriter() : sqlwriter() {
	debugFunction();
}

mysqlsqlwriter::~mysqlsqlwriter() {
	debugFunction();
}

const char * const *mysqlsqlwriter::additionalElements() {
	debugFunction();
	static const char *additionalelements[]={
		NULL
	};
	return additionalelements;
}
