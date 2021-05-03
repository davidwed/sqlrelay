// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrresultsettable.h>

class sqlrresultsettableprivate {
	private:
		friend class sqlrresultsettable;
		sqlrcursor	*cursor;
};

sqlrresultsettable::sqlrresultsettable() : tablecollection<const char *>() {
	pvt=new sqlrresultsettableprivate;
	pvt->cursor=NULL;
}

sqlrresultsettable::~sqlrresultsettable() {
	delete pvt->cursor;
	delete[] pvt;
}

void sqlrresultsettable::attachCursor(sqlrcursor *cursor) {
	pvt->cursor=cursor;
}

uint64_t sqlrresultsettable::getRowCount() {
	return (pvt->cursor)?pvt->cursor->rowCount():0;
}

uint64_t sqlrresultsettable::getColCount() {
	return (pvt->cursor)?pvt->cursor->colCount():0;
}

void sqlrresultsettable::setValue(uint64_t row,
					uint64_t col,
					const char *value) {
}

const char *sqlrresultsettable::getValue(uint64_t row, uint64_t col) {
	return (pvt->cursor)?pvt->cursor->getField(row,col):"";
}

void sqlrresultsettable::setColumnName(uint64_t col, const char *name) {
	// do nothing
}

const char *sqlrresultsettable::getColumnName(uint64_t col) {
	return (pvt->cursor)?pvt->cursor->getColumnName(col):"";
}

void sqlrresultsettable::clear() {
	// do nothing
}
