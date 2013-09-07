// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/private/sqlrdefines.h>
#include <sqlrelay/private/row.h>
#include <rudiments/null.h>

#include <config.h>

row::row(uint32_t colcount) {
	this->colcount=colcount;
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new uint32_t
					[colcount-OPTIMISTIC_COLUMN_COUNT];
	} else {
		extrafields=NULL;
		extrafieldlengths=NULL;
	}
}

row::~row() {
	delete[] extrafields;
	delete[] extrafieldlengths;
}

void row::resize(uint32_t colcount) {
	this->colcount=colcount;
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		delete[] extrafields;
		delete[] extrafieldlengths;
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new uint32_t
					[colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

void row::addField(uint32_t column, const char *buffer, uint32_t length) {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		fields[column]=(char *)buffer;
		fieldlengths[column]=length;
	} else {
		extrafields[column-OPTIMISTIC_COLUMN_COUNT]=(char *)buffer;
		extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT]=length;
	}
}

char *row::getField(uint32_t column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fields[column];
	} else {
		return extrafields[column-OPTIMISTIC_COLUMN_COUNT];
	}
}

uint32_t row::getFieldLength(uint32_t column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fieldlengths[column];
	} else {
		return extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT];
	}
}
