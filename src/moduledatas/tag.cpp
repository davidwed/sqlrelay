// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>

#include "sqlrmoduledata_tag.h"

sqlrmd_tag::sqlrmd_tag(domnode *parameters) : sqlrmoduledata(parameters) {
}

sqlrmd_tag::~sqlrmd_tag() {
	tagmap.clearAndArrayDeleteValues();
}

void sqlrmd_tag::setTag(uint16_t cursorid, const char *tag) {
	tagmap.removeAndArrayDeleteValue(cursorid);
	tagmap.setValue(cursorid,charstring::duplicate(tag));
}

void sqlrmd_tag::setTag(uint16_t cursorid, const char *tag, size_t size) {
	tagmap.removeAndArrayDeleteValue(cursorid);
	tagmap.setValue(cursorid,charstring::duplicate(tag,size));
}

const char *sqlrmd_tag::getTag(uint16_t cursorid) {
	return tagmap.getValue(cursorid);;
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrmoduledata	*new_sqlrmoduledata_tag(domnode *parameters) {
		return new sqlrmd_tag(parameters);
	}
}
