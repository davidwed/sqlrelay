
// Copyright (c) 1999-2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrcollections.h>

class sqlrlinkedlistnodeprivate {
	private:
		friend class sqlrlinkedlistnode;

		sqlrcursor	*_cursor;
		bool		_representsarow;
		uint64_t	_row;
		uint32_t	_col;

		const char	*_null;
};

sqlrlinkedlistnode::sqlrlinkedlistnode() : listnode<const char *>() {
	pvt=new sqlrlinkedlistnodeprivate;
	pvt->_cursor=NULL;
	pvt->_representsarow=true;
	pvt->_row=0;
	pvt->_col=0;
	pvt->_null=NULL;
}

sqlrlinkedlistnode::~sqlrlinkedlistnode() {
}

void sqlrlinkedlistnode::setCursor(sqlrcursor *cursor) {
	pvt->_cursor=cursor;
	pvt->_row=0;
	pvt->_col=0;
}

sqlrcursor *sqlrlinkedlistnode::getCursor() const {
	return pvt->_cursor;
}

void sqlrlinkedlistnode::setRepresentsARow(bool representsarow) {
	pvt->_representsarow=representsarow;
}

void sqlrlinkedlistnode::setRow(uint64_t row) {
	pvt->_row=row;
	if (pvt->_representsarow) {
		pvt->_col=0;
	}
}

void sqlrlinkedlistnode::setColumn(uint32_t col) {
	pvt->_col=col;
	if (!pvt->_representsarow) {
		pvt->_row=0;
	}
}

void sqlrlinkedlistnode::setValue(const char *value) {
	// do nothing
}

const char *sqlrlinkedlistnode::getValue() const {
	return pvt->_cursor->getField(pvt->_row,pvt->_col);
}

const char * &sqlrlinkedlistnode::getValue() {
	return pvt->_null;
}

void sqlrlinkedlistnode::setNext(listnode<const char *> *next) {
	// do nothing
}

void sqlrlinkedlistnode::setPrevious(listnode<const char *> *next) {
	// do nothing
}

listnode<const char *> *sqlrlinkedlistnode::getPrevious() const {
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

listnode<const char *> *sqlrlinkedlistnode::getNext() const {
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

class sqlrrowlinkedlistprivate {
	private:
		friend class sqlrrowlinkedlist;

		sqlrcursor		*_cursor;
		sqlrlinkedlistnode	_node;
};

sqlrrowlinkedlist::sqlrrowlinkedlist() : listcollection<const char *>() {
	pvt=new sqlrrowlinkedlistprivate;
	pvt->_cursor=NULL;
}

sqlrrowlinkedlist::sqlrrowlinkedlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrrowlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
}

sqlrrowlinkedlist::sqlrrowlinkedlist(sqlrcursor *cursor, uint64_t row) :
					listcollection<const char *>() {
	pvt=new sqlrrowlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRow(row);
}

sqlrrowlinkedlist::~sqlrrowlinkedlist() {
}

void sqlrrowlinkedlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrrowlinkedlist::setRow(uint64_t row) {
	pvt->_node.setRow(row);
}

bool sqlrrowlinkedlist::getIsReadOnly() const {
	return true;
}

bool sqlrrowlinkedlist::getIsBlockBased() const {
	return true;
}

uint64_t sqlrrowlinkedlist::getBlockSize() const {
	return 1;
}

void sqlrrowlinkedlist::prepend(const char *value) {
	// do nothing
}

void sqlrrowlinkedlist::prepend(listnode<const char *> *node) {
	// do nothing
}

void sqlrrowlinkedlist::append(const char *value) {
	// do nothing
}

void sqlrrowlinkedlist::append(listnode<const char *> *node) {
	// do nothing
}

void sqlrrowlinkedlist::insertBefore(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrrowlinkedlist::insertBefore(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrrowlinkedlist::insertAfter(listnode<const char *> *node,
							const char *value) {
	// do nothing
}

void sqlrrowlinkedlist::insertAfter(listnode<const char *> *node,
					listnode<const char *> *newnode) {
	// do nothing
}

void sqlrrowlinkedlist::moveBefore(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrrowlinkedlist::moveAfter(listnode<const char *> *node,
					listnode<const char *> *nodetomove) {
	// do nothing
}

void sqlrrowlinkedlist::detach(listnode<const char *> *node) {
	// do nothing
}

bool sqlrrowlinkedlist::remove(const char *value) {
	// do nothing
	return true;
}

bool sqlrrowlinkedlist::removeAll(const char *value) {
	// do nothing
	return true;
}

bool sqlrrowlinkedlist::remove(listnode<const char *> *node) {
	// do nothing
	return true;
}

uint64_t sqlrrowlinkedlist::getLength() const {
	return pvt->_node.getCursor()->colCount();
}

listnode<const char *> *sqlrrowlinkedlist::getFirst() const {
	pvt->_node.setColumn(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrrowlinkedlist::getNext(
					listnode<const char *> *node) const {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrrowlinkedlist::find(const char *value) const {
	pvt->_node.setColumn(0);
	for (listnode<const char *> *n=&pvt->_node; n; n=n->getNext()) {
		if (getComparator()->compare(value,n->getValue())) {
			return n;
		}
	}
	return NULL;
}

listnode<const char *> *sqlrrowlinkedlist::find(
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

void sqlrrowlinkedlist::insertionSort() {
	// do nothing
}

void sqlrrowlinkedlist::heapSort() {
	// do nothing
}

void sqlrrowlinkedlist::clear() {
	// do nothing
}

class sqlrresultsetlinkedlistprivate {
	private:
		friend class sqlrresultsetlinkedlist;

		sqlrcursor		*_cursor;
		sqlrlinkedlistnode	_node;
};

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist() :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=NULL;
	pvt->_node.setRepresentsARow(false);
}

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist(sqlrcursor *cursor) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRepresentsARow(false);
}

sqlrresultsetlinkedlist::sqlrresultsetlinkedlist(sqlrcursor *cursor,
							uint64_t col) :
					listcollection<const char *>() {
	pvt=new sqlrresultsetlinkedlistprivate;
	pvt->_cursor=cursor;
	pvt->_node.setCursor(cursor);
	pvt->_node.setRepresentsARow(false);
	pvt->_node.setColumn(col);
}

sqlrresultsetlinkedlist::~sqlrresultsetlinkedlist() {
}

void sqlrresultsetlinkedlist::setCursor(sqlrcursor *cursor) {
	pvt->_node.setCursor(cursor);
}

void sqlrresultsetlinkedlist::setColumn(uint32_t col) {
	pvt->_node.setColumn(col);
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
	pvt->_node.setRow(0);
	return &pvt->_node;
}

listnode<const char *> *sqlrresultsetlinkedlist::getNext(
					listnode<const char *> *node) const {
	return pvt->_node.getNext();
}

listnode<const char *> *sqlrresultsetlinkedlist::find(const char *value) const {
	pvt->_node.setRow(0);
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
