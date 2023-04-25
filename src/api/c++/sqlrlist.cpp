
// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrlistnodeprivate {
	private:
		friend class sqlrlistnode;

		sqlrcursor	*_cursor;
		bool		_representsarow;
		uint64_t	_row;
		uint32_t	_col;

		const char	*_null;
};

sqlrlistnode::sqlrlistnode() : listnode<const char *>() {
	pvt=new sqlrlistnodeprivate;
	pvt->_cursor=NULL;
	pvt->_representsarow=true;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrlistnode::~sqlrlistnode() {
}

void sqlrlistnode::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrcursor *sqlrlistnode::getCursor() {
	return pvt->_cursor;
}

void sqlrlistnode::setRepresentsARow(bool representsarow) {
	pvt->_representsarow=representsarow;
}

void sqlrlistnode::setRow(uint64_t row) {
	pvt->_row=row;
	if (pvt->_representsarow) {
		pvt->_col=0;
	}
}

void sqlrlistnode::setColumn(uint32_t col) {
	pvt->_col=col;
	if (!pvt->_representsarow) {
		pvt->_row=0;
	}
}

void sqlrlistnode::setValue(const char *value) {
	// do nothing
}

const char *sqlrlistnode::getValue() {
	return pvt->_cursor->getField(pvt->_row,pvt->_col);
}

const char * &sqlrlistnode::getReference() {
	return pvt->_null;
}

void sqlrlistnode::setNext(listnode<const char *> *next) {
	// do nothing
}

void sqlrlistnode::setPrevious(listnode<const char *> *next) {
	// do nothing
}

listnode<const char *> *sqlrlistnode::getPrevious() {
	if (!pvt->_col) {
		return NULL;
	} else {
		if (pvt->_representsarow) {
			pvt->_col--;
		} else {
			pvt->_row--;
		}
		return (listnode<const char *> *)this;
	} 
}

listnode<const char *> *sqlrlistnode::getNext() {
	if (pvt->_col==pvt->_cursor->colCount()-1) {
		return NULL;
	} else {
		if (pvt->_representsarow) {
			pvt->_col++;
		} else {
			pvt->_row++;
		}
		return (listnode<const char *> *)this;
	} 
}

class sqlrrowlistprivate {
	private:
		friend class sqlrrowlist;

		sqlrcursor		*_cursor;
		sqlrlistnode	_node;
};

sqlrrowlist::sqlrrowlist() : listcollection<const char *>() {
	pvt=new sqlrrowlistprivate;
	pvt->_cursor=NULL;
}

sqlrrowlist::sqlrrowlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrrowlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
}

sqlrrowlist::sqlrrowlist(sqlrcursor *cursor, uint64_t row) :
					listcollection<const char *>() {
	pvt=new sqlrrowlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRow(row);
}

sqlrrowlist::~sqlrrowlist() {
}

void sqlrrowlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrrowlist::setRow(uint64_t row) {
	pvt->_node.setRow(row);
}

bool sqlrrowlist::getIsReadOnly() {
	return true;
}

bool sqlrrowlist::getIsBlockBased() {
	return true;
}

uint64_t sqlrrowlist::getBlockSize() {
	return 1;
}

void sqlrrowlist::prepend(const char *value) {
	// do nothing
}

void sqlrrowlist::prepend(listnode<const char *> *node) {
	// do nothing
}

void sqlrrowlist::append(const char *value) {
	// do nothing
}

void sqlrrowlist::append(listnode<const char *> *node) {
	// do nothing
}

void sqlrrowlist::insertBefore(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrrowlist::insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrrowlist::insertAfter(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrrowlist::insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrrowlist::moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrrowlist::moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrrowlist::detach(listnode<const char *> *node) {
	// do nothing
}

bool sqlrrowlist::remove(const char *value) {
	// do nothing
	return true;
}

bool sqlrrowlist::removeAll(const char *value) {
	// do nothing
	return true;
}

bool sqlrrowlist::remove(listnode<const char *> *node) {
	// do nothing
	return true;
}

uint64_t sqlrrowlist::getLength() {
	return pvt->_node.getCursor()->colCount();
}

listnode<const char *> *sqlrrowlist::getFirst() {
	pvt->_node.setColumn(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrrowlist::getNext(
					listnode<const char *> *node) {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrrowlist::find(const char *value) {
	pvt->_node.setColumn(0);
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

listnode<const char *> *sqlrrowlist::find(
					listnode<const char *> *startnode,
					const char *value) {
	// since this implementation is block-based, we can't really start at
	// a specific node, so we'll just start at the current node and search
	// from there
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

void sqlrrowlist::sortInexpensively() {
	// do nothing
}

void sqlrrowlist::sortQuickly() {
	// do nothing
}

bool sqlrrowlist::clear() {
	// do nothing
	return true;
}

class sqlrresultsetlistprivate {
	private:
		friend class sqlrresultsetlist;

		sqlrcursor	*_cursor;
		sqlrlistnode	_node;
};

sqlrresultsetlist::sqlrresultsetlist() : listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=NULL;
	pvt->_node.setRepresentsARow(false);
}

sqlrresultsetlist::sqlrresultsetlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRepresentsARow(false);
}

sqlrresultsetlist::sqlrresultsetlist(sqlrcursor *cursor, uint64_t col) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRepresentsARow(false);
	pvt->_node.setColumn(col);
}

sqlrresultsetlist::~sqlrresultsetlist() {
}

void sqlrresultsetlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrresultsetlist::setColumn(uint32_t col) {
	pvt->_node.setColumn(col);
}

bool sqlrresultsetlist::getIsReadOnly() {
	return true;
}

bool sqlrresultsetlist::getIsBlockBased() {
	return true;
}

uint64_t sqlrresultsetlist::getBlockSize() {
	return 1;
}

void sqlrresultsetlist::prepend(const char *value) {
	// do nothing
}

void sqlrresultsetlist::prepend(listnode<const char *> *node) {
	// do nothing
}

void sqlrresultsetlist::append(const char *value) {
	// do nothing
}

void sqlrresultsetlist::append(listnode<const char *> *node) {
	// do nothing
}

void sqlrresultsetlist::insertBefore(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrresultsetlist::insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrresultsetlist::insertAfter(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrresultsetlist::insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrresultsetlist::moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrresultsetlist::moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrresultsetlist::detach(listnode<const char *> *node) {
	// do nothing
}

bool sqlrresultsetlist::remove(const char *value) {
	// do nothing
	return true;
}

bool sqlrresultsetlist::removeAll(const char *value) {
	// do nothing
	return true;
}

bool sqlrresultsetlist::remove(listnode<const char *> *node) {
	// do nothing
	return true;
}

uint64_t sqlrresultsetlist::getLength() {
	return pvt->_node.getCursor()->colCount();
}

listnode<const char *> *sqlrresultsetlist::getFirst() {
	pvt->_node.setRow(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrresultsetlist::getNext(
					listnode<const char *> *node) {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrresultsetlist::find(const char *value) {
	pvt->_node.setRow(0);
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

listnode<const char *> *sqlrresultsetlist::find(
					listnode<const char *> *startnode,
					const char *value) {
	// since this implementation is block-based, we can't really start at
	// a specific node, so we'll just start at the current node and search
	// from there
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

void sqlrresultsetlist::sortInexpensively() {
	// do nothing
}

void sqlrresultsetlist::sortQuickly() {
	// do nothing
}

bool sqlrresultsetlist::clear() {
	// do nothing
	return true;
}
