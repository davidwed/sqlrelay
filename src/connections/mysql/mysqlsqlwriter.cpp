// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <mysqlsqlwriter.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

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
