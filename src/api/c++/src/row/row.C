// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/private/sqlrdefines.h>
#include <sqlrelay/private/row.h>
#include <stdlib.h>

#include <config.h>

row::row(int colcount) {
	this->colcount=colcount;
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new unsigned long
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

void row::resize(int colcount) {
	if (colcount>=OPTIMISTIC_COLUMN_COUNT) {
		delete[] extrafields;
		delete[] extrafieldlengths;
		extrafields=new char *[colcount-OPTIMISTIC_COLUMN_COUNT];
		extrafieldlengths=new unsigned long
					[colcount-OPTIMISTIC_COLUMN_COUNT];
	}
}

void row::addField(int column, const char *buffer, unsigned long length) {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		fields[column]=(char *)buffer;
		fieldlengths[column]=length;
	} else {
		extrafields[column-OPTIMISTIC_COLUMN_COUNT]=(char *)buffer;
		extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT]=length;
	}
}

char *row::getField(int column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fields[column];
	} else {
		return extrafields[column-OPTIMISTIC_COLUMN_COUNT];
	}
}

unsigned long row::getFieldLength(int column) const {
	if (column<OPTIMISTIC_COLUMN_COUNT) {
		return fieldlengths[column];
	} else {
		return extrafieldlengths[column-OPTIMISTIC_COLUMN_COUNT];
	}
}
