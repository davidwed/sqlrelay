// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlparser.h>
#include <debugprint.h>

bool sqlparser::space(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr," ");
}

bool sqlparser::comma(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,",");
}

bool sqlparser::equals(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"=");
}

bool sqlparser::notEquals(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"!=");
}

bool sqlparser::lessThan(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"<");
}

bool sqlparser::greaterThan(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,">");
}

bool sqlparser::lessThanOrEqualTo(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"<=");
}

bool sqlparser::greaterThanOrEqualTo(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,">=");
}

bool sqlparser::leftParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"(");
}

bool sqlparser::rightParen(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,")");
}

bool sqlparser::compliment(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"~");
}

bool sqlparser::inverse(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"!");
}

bool sqlparser::plus(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"+");
}

bool sqlparser::minus(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"-");
}

bool sqlparser::times(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"*");
}

bool sqlparser::dividedBy(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"/");
}

bool sqlparser::bitwiseAnd(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"&");
}

bool sqlparser::bitwiseOr(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"|");
}

bool sqlparser::bitwiseXor(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"^");
}

bool sqlparser::logicalAnd(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"&&");
}

bool sqlparser::logicalOr(const char *ptr, const char **newptr) {
	debugFunction();
	return comparePart(ptr,newptr,"||");
}
