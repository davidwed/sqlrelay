
// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrresultsetlinkedlistnodeprivate {
	private:
		friend class sqlrresultsetlinkedlistnode;

		sqlrcursor	*_cursor;
		uint64_t	_row;
		uint32_t	_col;

		const char	*_null;
};

sqlrresultsetlinkedlistnode::sqlrresultsetlinkedlistnode() :
						listnode<const char *>() {
	pvt=new sqlrresultsetlinkedlistnodeprivate;
	pvt->_cursor=NULL;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlinkedlistnode::sqlrresultsetlinkedlistnode(sqlrcursor *cursor) :
						listnode<const char *>() {
	pvt=new sqlrresultsetlinkedlistnodeprivate;
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlinkedlistnode::sqlrresultsetlinkedlistnode(sqlrcursor *cursor,
							uint64_t row) :
						listnode<const char *>() {
	pvt=new sqlrresultsetlinkedlistnodeprivate;
	pvt->_cursor=cursor;
	pvt->_row=row;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlinkedlistnode::~sqlrresultsetlinkedlistnode() {
}

void sqlrresultsetlinkedlistnode::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrcursor *sqlrresultsetlinkedlistnode::getCursor() const {
	return pvt->_cursor;
}

void sqlrresultsetlinkedlistnode::setRow(uint64_t row) {
	pvt->_row=row;
	pvt->_col=0;
}

void sqlrresultsetlinkedlistnode::setColumn(uint32_t col) {
	pvt->_col=col;
}

void sqlrresultsetlinkedlistnode::setValue(const char *value) {
	// do nothing
}

const char *sqlrresultsetlinkedlistnode::getValue() const {
	return pvt->_cursor->getField(pvt->_row,pvt->_col);
}

const char * &sqlrresultsetlinkedlistnode::getValue() {
	return pvt->_null;
}

void sqlrresultsetlinkedlistnode::setNext(listnode<const char *> *next) {
	// do nothing
}

void sqlrresultsetlinkedlistnode::setPrevious(listnode<const char *> *next) {
	// do nothing
}

listnode<const char *> *sqlrresultsetlinkedlistnode::getPrevious() const {
	if (!pvt->_col) {
		return NULL;
	} else {
		pvt->_row--;
		return (listnode<const char *> *)this;
	} 
}

listnode<const char *> *sqlrresultsetlinkedlistnode::getNext() const {
	if (pvt->_col==pvt->_cursor->colCount()-1) {
		return NULL;
	} else {
		pvt->_row++;
		return (listnode<const char *> *)this;
	} 
}

void sqlrresultsetlinkedlistnode::print() const {
	// FIXME: implement this
}

class sqlrresultsetlinkedlistprivate {
	private:
		friend class sqlrresultsetlinkedlist;

		sqlrcursor		*_cursor;
		sqlrresultsetlinkedlistnode	_node;
};

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist() : listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=NULL;
}

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
}

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist(sqlrcursor *cursor, uint64_t row) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRow(row);
}

sqlrresultsetlinkedlist::~sqlrresultsetlinkedlist() {
}

void sqlrresultsetlinkedlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrresultsetlinkedlist::setRow(uint64_t row) {
	pvt->_node.setRow(row);
}

bool sqlrresultsetlinkedlist::getIsReadOnly() const {
	return true;
}

bool sqlrresultsetlinkedlist::getIsBlockBased() const {
	return true;
}

uint64_t sqlrresultsetlinkedlist::getBlockSize() const {
	return 1;
}

void sqlrresultsetlinkedlist::prepend(const char *value) {
	// do nothing
}

void sqlrresultsetlinkedlist::prepend(listnode<const char *> *node) {
	// do nothing
}

void sqlrresultsetlinkedlist::append(const char *value) {
	// do nothing
}

void sqlrresultsetlinkedlist::append(listnode<const char *> *node) {
	// do nothing
}

void sqlrresultsetlinkedlist::insertBefore(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrresultsetlinkedlist::insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrresultsetlinkedlist::insertAfter(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrresultsetlinkedlist::insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrresultsetlinkedlist::moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrresultsetlinkedlist::moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrresultsetlinkedlist::detach(listnode<const char *> *node) {
	// do nothing
}

bool sqlrresultsetlinkedlist::remove(const char *value) {
	// do nothing
	return true;
}

bool sqlrresultsetlinkedlist::removeAll(const char *value) {
	// do nothing
	return true;
}

bool sqlrresultsetlinkedlist::remove(listnode<const char *> *node) {
	// do nothing
	return true;
}

uint64_t sqlrresultsetlinkedlist::getLength() const {
	return pvt->_node.getCursor()->colCount();
}

listnode<const char *> *sqlrresultsetlinkedlist::getFirst() const {
	pvt->_node.setColumn(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrresultsetlinkedlist::getNext(
					listnode<const char *> *node) const {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrresultsetlinkedlist::find(const char *value) const {
	pvt->_node.setColumn(0);
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

listnode<const char *> *sqlrresultsetlinkedlist::find(
					listnode<const char *> *startnode,
					const char *value) const {
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

void sqlrresultsetlinkedlist::insertionSort() {
	// do nothing
}

void sqlrresultsetlinkedlist::heapSort() {
	// do nothing
}

void sqlrresultsetlinkedlist::clear() {
	// do nothing
}

void sqlrresultsetlinkedlist::print() const {
	// FIXME: implement this
}

void sqlrresultsetlinkedlist::print(uint64_t count) const {
	// FIXME: implement this
}
