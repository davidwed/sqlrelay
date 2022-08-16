// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrresultsettableprivate {
	private:
		friend class sqlrresultsettable;
		sqlrcursor	*_cursor;
};

sqlrresultsettable::sqlrresultsettable() : tablecollection<const char *>() {
	pvt=new sqlrresultsettableprivate;
	pvt->_cursor=NULL;
}

sqlrresultsettable::sqlrresultsettable(sqlrcursor *cursor) :
					tablecollection<const char *>() {
	pvt=new sqlrresultsettableprivate;
	pvt->_cursor=cursor;
}

sqlrresultsettable::~sqlrresultsettable() {
	delete pvt;
}

void sqlrresultsettable::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
}

bool sqlrresultsettable::getIsReadOnly() {
	return true;
}

bool sqlrresultsettable::getIsBlockBased() {
	return true;
}

bool sqlrresultsettable::getIsSequentialAccess() {
	return true;
}

void sqlrresultsettable::setColumnName(uint64_t col, const char *name) {
	// do nothing
}

const char *sqlrresultsettable::getColumnName(uint64_t col) {
	return (pvt->_cursor)?pvt->_cursor->getColumnName(col):"";
}

uint64_t sqlrresultsettable::getColumnCount() {
	return (pvt->_cursor)?pvt->_cursor->colCount():0;
}

void sqlrresultsettable::setValue(uint64_t row,
					uint64_t col,
					const char *value) {
}

const char *sqlrresultsettable::getValue(uint64_t row,
						uint64_t col) {
	return (pvt->_cursor)?pvt->_cursor->getField(row,col):"";
}

const char *sqlrresultsettable::getValue(uint64_t row,
						const char *colname) {
	return (pvt->_cursor)?pvt->_cursor->getField(row,colname):"";
}

uint64_t sqlrresultsettable::getRowCount() {
	return (pvt->_cursor)?pvt->_cursor->rowCount():0;
}

uint64_t sqlrresultsettable::getBlockSize() {
	return (pvt->_cursor)?pvt->_cursor->getResultSetBufferSize():0;
}

bool sqlrresultsettable::getAllRowsAvailable() {
	return (pvt->_cursor)?pvt->_cursor->endOfResultSet():true;
}

void sqlrresultsettable::clear() {
	// do nothing
}
