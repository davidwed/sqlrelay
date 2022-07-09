
// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrresultsetlistnodeprivate {
	private:
		friend class sqlrresultsetlistnode;

		sqlrcursor	*_cursor;
		uint64_t	_row;
		uint32_t	_col;

		const char	*_null;
};

sqlrresultsetlistnode::sqlrresultsetlistnode() : listnode<const char *>() {
	pvt=new sqlrresultsetlistnodeprivate;
	pvt->_cursor=NULL;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlistnode::sqlrresultsetlistnode(sqlrcursor *cursor) :
						listnode<const char *>() {
	pvt=new sqlrresultsetlistnodeprivate;
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlistnode::sqlrresultsetlistnode(sqlrcursor *cursor, uint64_t row) :
						listnode<const char *>() {
	pvt=new sqlrresultsetlistnodeprivate;
	pvt->_cursor=cursor;
	pvt->_row=row;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrresultsetlistnode::~sqlrresultsetlistnode() {
}

void sqlrresultsetlistnode::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrcursor *sqlrresultsetlistnode::getCursor() const {
	return pvt->_cursor;
}

void sqlrresultsetlistnode::setRow(uint64_t row) {
	pvt->_row=row;
	pvt->_col=0;
}

void sqlrresultsetlistnode::setColumn(uint32_t col) {
	pvt->_col=col;
}

void sqlrresultsetlistnode::setValue(const char *value) {
	// do nothing
}

const char *sqlrresultsetlistnode::getValue() const {
	return pvt->_cursor->getField(pvt->_row,pvt->_col);
}

const char * &sqlrresultsetlistnode::getValue() {
	return pvt->_null;
}

void sqlrresultsetlistnode::setNext(listnode<const char *> *next) {
	// do nothing
}

void sqlrresultsetlistnode::setPrevious(listnode<const char *> *next) {
	// do nothing
}

listnode<const char *> *sqlrresultsetlistnode::getPrevious() const {
	if (!pvt->_col) {
		return NULL;
	} else {
		pvt->_row--;
		return (listnode<const char *> *)this;
	} 
}

listnode<const char *> *sqlrresultsetlistnode::getNext() const {
	if (pvt->_col==pvt->_cursor->colCount()-1) {
		return NULL;
	} else {
		pvt->_row++;
		return (listnode<const char *> *)this;
	} 
}

class sqlrresultsetlistprivate {
	private:
		friend class sqlrresultsetlist;

		sqlrcursor		*_cursor;
		sqlrresultsetlistnode	_node;
};

sqlrresultsetlist::sqlrresultsetlist() : listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=NULL;
}

sqlrresultsetlist::sqlrresultsetlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
}

sqlrresultsetlist::sqlrresultsetlist(sqlrcursor *cursor, uint64_t row) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRow(row);
}

sqlrresultsetlist::~sqlrresultsetlist() {
}

void sqlrresultsetlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrresultsetlist::setRow(uint64_t row) {
	pvt->_node.setRow(row);
}

bool sqlrresultsetlist::getIsReadOnly() const {
	return true;
}

bool sqlrresultsetlist::getIsBlockBased() const {
	return true;
}

uint64_t sqlrresultsetlist::getBlockSize() const {
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

uint64_t sqlrresultsetlist::getLength() const {
	return pvt->_node.getCursor()->colCount();
}

listnode<const char *> *sqlrresultsetlist::getFirst() const {
	pvt->_node.setColumn(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrresultsetlist::getNext(
					listnode<const char *> *node) const {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrresultsetlist::find(const char *value) const {
	pvt->_node.setColumn(0);
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

listnode<const char *> *sqlrresultsetlist::find(
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

void sqlrresultsetlist::insertionSort() {
	// do nothing
}

void sqlrresultsetlist::heapSort() {
	// do nothing
}

void sqlrresultsetlist::clear() {
	// do nothing
}

ssize_t sqlrresultsetlist::write(output *out) const {
	// FIXME: implement this
	return 0;
}

ssize_t sqlrresultsetlist::write(output *out, uint64_t count) const {
	// FIXME: implement this
	return 0;
}
