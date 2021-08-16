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

sqlrresultsettable::sqlrresultsettable(sqlrcursor *cursor) :
					tablecollection<const char *>() {
	pvt=new sqlrresultsettableprivate;
	pvt->cursor=cursor;
}

sqlrresultsettable::~sqlrresultsettable() {
	delete pvt;
}

void sqlrresultsettable::setCursor(sqlrcursor *cursor) {
	pvt->cursor=cursor;
}

void sqlrresultsettable::setColumnName(uint64_t col, const char *name) {
	// do nothing
}

const char *sqlrresultsettable::getColumnName(uint64_t col) {
	return (pvt->cursor)?pvt->cursor->getColumnName(col):"";
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

const char *sqlrresultsettable::getValue(uint64_t row, const char *colname) {
	return (pvt->cursor)?pvt->cursor->getField(row,colname):"";
}

uint64_t sqlrresultsettable::getRowCount() {
	return (pvt->cursor)?pvt->cursor->rowCount():0;
}

bool sqlrresultsettable::allRowsAvailable() {
	return (pvt->cursor)?pvt->cursor->endOfResultSet():true;
}

void sqlrresultsettable::clear() {
	// do nothing
}
