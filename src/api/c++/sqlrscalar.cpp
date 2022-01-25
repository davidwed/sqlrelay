// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrscalarprivate {
	private:
		friend class sqlrscalar;

		sqlrcursor	*_cursor;
		uint64_t	_row;
		uint32_t	_col;
};

sqlrscalar::sqlrscalar() : scalarcollection<const char *>() {
	pvt=new sqlrscalarprivate;
	pvt->_cursor=NULL;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrscalar::sqlrscalar(sqlrcursor *cursor) :
					scalarcollection<const char *>() {
	pvt=new sqlrscalarprivate;
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrscalar::sqlrscalar(sqlrcursor *cursor, uint64_t row, uint64_t col) :
					scalarcollection<const char *>() {
	pvt=new sqlrscalarprivate;
	pvt->_cursor=cursor;
	pvt->_row=row;
	pvt->_col=col;
}

sqlrscalar::~sqlrscalar() {
}

bool sqlrscalar::getIsReadOnly() const {
	return true;
}

void sqlrscalar::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

void sqlrscalar::setRow(uint64_t row) {
	pvt->_row=row;
}

void sqlrscalar::setColumn(uint32_t col) {
	pvt->_col=col;
}

void sqlrscalar::setValue(const char *value) {
	// do nothing
}

const char *sqlrscalar::getValue() const {
	return pvt->_cursor->getField(pvt->_row,pvt->_col);
}

void sqlrscalar::clear() {
	// do nothing
}
