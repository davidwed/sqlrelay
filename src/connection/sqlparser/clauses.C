// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <sqltranslatordebug.h>

bool sqlparser::space(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr," ");
}

bool sqlparser::comma(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,",");
}

bool sqlparser::leftParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"(");
}

bool sqlparser::rightParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,")");
}
