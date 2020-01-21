// Copyright (c) 2018 David Muse
// See the COPYING file for more information.

#include <sqlrelay/sqlrresultsetdomnode.h>

enum sqlrresultsetdomnodestate {
	RESULTSET,
	AFFECTEDROWS,
	COLUMNS,
	COLUMN,
	ROWS,
	ROW,
	FIELD,
	ATTR
};

class sqlrresultsetdomnodeprivate {
	friend class sqlrresultsetdomnode;
	private:
		sqlrcursor	*_sqlrcur;

		uint64_t	_col;
		uint64_t	_row;
		uint64_t	_field;

		char		*_affectedrows;
		
		sqlrresultsetdomnodestate	_state;

		sqlrresultsetdomnode	*_attrnode;
};

sqlrresultsetdomnode::sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					sqlrcursor *sqlrcur) :
						cursordomnode(dom,nullnode) {
	init(true,sqlrcur);
}

sqlrresultsetdomnode::sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					const char *ns,
					sqlrcursor *sqlrcur) :
						cursordomnode(dom,nullnode,ns) {
	init(true,sqlrcur);
}

sqlrresultsetdomnode::sqlrresultsetdomnode(dom *dom,
					domnode *nullnode,
					sqlrcursor *sqlrcur,
					bool createattrnode) :
						cursordomnode(dom,nullnode) {
	init(createattrnode,sqlrcur);
}

sqlrresultsetdomnode::~sqlrresultsetdomnode() {
	delete[] pvt->_affectedrows;
	delete pvt->_attrnode;
	delete pvt;
}

void sqlrresultsetdomnode::init(bool createattrnode, sqlrcursor *sqlrcur) {
	pvt=new sqlrresultsetdomnodeprivate;
	pvt->_sqlrcur=sqlrcur;
	pvt->_state=RESULTSET;
	pvt->_col=0;
	pvt->_row=0;
	pvt->_field=0;
	pvt->_affectedrows=NULL;
	if (createattrnode) {
		pvt->_attrnode=new sqlrresultsetdomnode(
				getTree(),getNullNode(),sqlrcur,false);
		pvt->_attrnode->pvt->_state=ATTR;
		pvt->_attrnode->setParent(this);
	} else {
		pvt->_attrnode=NULL;
	}
}

void sqlrresultsetdomnode::setSqlrCursor(sqlrcursor *sqlrcur) {
	pvt->_sqlrcur=sqlrcur;
}

sqlrcursor *sqlrresultsetdomnode::getSqlrCursor() {
	return pvt->_sqlrcur;
}

domnodetype sqlrresultsetdomnode::getType() const {
	return (pvt->_state==ATTR)?ATTRIBUTE_DOMNODETYPE:TAG_DOMNODETYPE;
}

const char *sqlrresultsetdomnode::getName() const {
	switch (pvt->_state) {
		case RESULTSET:
			return "resultset";
		case AFFECTEDROWS:
			return "affectedrows";
		case COLUMNS:
			return "columns";
		case COLUMN:
			return "column";
		case ROWS:
			return "rows";
		case ROW:
			return "row";
		case FIELD:
			return "field";
		case ATTR:
			{
			sqlrresultsetdomnode *parent=
				(sqlrresultsetdomnode *)domnode::getParent();
			return (parent->pvt->_state==COLUMNS ||
					parent->pvt->_state==ROWS ||
					parent->pvt->_state==ROW)?"t":"value";
			}
		default:
			return NULL;
	}
}

const char *sqlrresultsetdomnode::getValue() const {
	if (pvt->_state==ATTR) {
		sqlrresultsetdomnode *parent=
				(sqlrresultsetdomnode *)domnode::getParent();
		if (parent->pvt->_state==AFFECTEDROWS) {
			if (!pvt->_affectedrows) {
				pvt->_affectedrows=
					charstring::parseNumber(
						pvt->_sqlrcur->affectedRows());
			}
			return pvt->_affectedrows;
		} else if (parent->pvt->_state==COLUMNS) {
			return "a";
		} else if (parent->pvt->_state==COLUMN) {
			return pvt->_sqlrcur->getColumnName(parent->pvt->_col);
		} else if (parent->pvt->_state==ROWS) {
			return "a";
		} else if (parent->pvt->_state==ROW) {
			return "a";
		} else if (parent->pvt->_state==FIELD) {
			return pvt->_sqlrcur->getField(parent->pvt->_row,
							parent->pvt->_field);
		}
	}
	return NULL;
}

domnode *sqlrresultsetdomnode::getParent() const {
	switch (pvt->_state) {
		case RESULTSET:
			return domnode::getParent();
		case AFFECTEDROWS:
			pvt->_state=RESULTSET;
			return (domnode *)this;
		case COLUMNS:
			pvt->_state=RESULTSET;
			return (domnode *)this;
		case COLUMN:
			pvt->_state=COLUMNS;
			return (domnode *)this;
		case ROWS:
			pvt->_state=RESULTSET;
			return (domnode *)this;
		case ROW:
			pvt->_state=ROWS;
			return (domnode *)this;
		case FIELD:
			pvt->_state=ROW;
			return (domnode *)this;
		case ATTR:
			return domnode::getParent();
		default:
			return NULL;
	}
}

uint64_t sqlrresultsetdomnode::getPosition() const {
	switch (pvt->_state) {
		case RESULTSET:
			return domnode::getPosition();
		case AFFECTEDROWS:
			return 0;
		case COLUMNS:
			return 1;
		case COLUMN:
			return 0;
		case ROWS:
			return 2;
		case ROW:
			return pvt->_row;
		case FIELD:
			return pvt->_field;
		case ATTR:
			return 0;
		default:
			return 0;
	}
}

domnode *sqlrresultsetdomnode::getPreviousSibling() const {
	switch (pvt->_state) {
		case RESULTSET:
			return domnode::getPreviousSibling();
		case AFFECTEDROWS:
			return getNullNode();
		case COLUMNS:
			pvt->_state=AFFECTEDROWS;
			return (domnode *)this;
		case COLUMN:
			return getNullNode();
		case ROWS:
			pvt->_state=COLUMNS;
			return (domnode *)this;
		case ROW:
			if (pvt->_row==0) {
				return getNullNode();
			} else {
				pvt->_row--;
				return (domnode *)this;
			}
		case FIELD:
			if (pvt->_field==0) {
				return getNullNode();
			} else {
				pvt->_field--;
				return (domnode *)this;
			}
		case ATTR:
			return getNullNode();
		default:
			return getNullNode();
	}
}

domnode *sqlrresultsetdomnode::getNextSibling() const {
	switch (pvt->_state) {
		case RESULTSET:
			return domnode::getNextSibling();
		case AFFECTEDROWS:
			pvt->_state=COLUMNS;
			return (domnode *)this;
		case COLUMNS:
			pvt->_state=ROWS;
			return (domnode *)this;
		case COLUMN:
			if ((pvt->_col+1)>=pvt->_sqlrcur->colCount()) {
				pvt->_state=COLUMNS;
				return getNullNode();
			} else {
				pvt->_col++;
				return (domnode *)this;
			}
		case ROWS:
			return getNullNode();
		case ROW:
			if (pvt->_sqlrcur->endOfResultSet() &&
				(pvt->_row+1)>=pvt->_sqlrcur->rowCount()) {
				pvt->_state=RESULTSET;
				return getNullNode();
			} else {
				pvt->_row++;
				return (domnode *)this;
			}
		case FIELD:
			if ((pvt->_field+1)>=pvt->_sqlrcur->colCount()) {
				pvt->_state=ROW;
				return getNullNode();
			} else {
				pvt->_field++;
				return (domnode *)this;
			}
		case ATTR:
			return getNullNode();
		default:
			return getNullNode();
	}
}

uint64_t sqlrresultsetdomnode::getChildCount() const {
	switch (pvt->_state) {
		case RESULTSET:
			return 2;
		case AFFECTEDROWS:
			return 0;
		case COLUMNS:
			return 1;
		case COLUMN:
			return 0;
		case ROWS:
			// of course, not reliable if there's
			// a result set buffer size
			return pvt->_sqlrcur->rowCount();
		case ROW:
			return pvt->_sqlrcur->colCount();
		case FIELD:
			return 1;
		case ATTR:
			return 0;
		default:
			return 0;
	}
}

domnode *sqlrresultsetdomnode::getFirstChild() const {
	switch (pvt->_state) {
		case RESULTSET:
			pvt->_state=AFFECTEDROWS;
			return (domnode *)this;
		case AFFECTEDROWS:
			return getNullNode();
		case COLUMNS:
			if (!pvt->_sqlrcur->colCount()) {
				return getNullNode();
			} else {
				pvt->_col=0;
				pvt->_state=COLUMN;
				return (domnode *)this;
			}
		case COLUMN:
			return getNullNode();
		case ROWS:
			if (pvt->_sqlrcur->endOfResultSet() &&
					!pvt->_sqlrcur->rowCount()) {
				return getNullNode();
			} else {
				pvt->_state=ROW;
				pvt->_row=0;
				return (domnode *)this;
			}
		case ROW:
			pvt->_state=FIELD;
			pvt->_field=0;
			return (domnode *)this;
		case FIELD:
			return getNullNode();
		case ATTR:
			return getNullNode();
		default:
			return getNullNode();
	}
}

uint64_t sqlrresultsetdomnode::getAttributeCount() const {
	switch (pvt->_state) {
		case RESULTSET:
		case ATTR:
			return 0;
		case AFFECTEDROWS:
		case COLUMNS:
		case COLUMN:
		case ROWS:
		case ROW:
		case FIELD:
			return 1;
		default:
			return 0;
	}
}

domnode *sqlrresultsetdomnode::getAttribute(const char *name) const {
	switch (pvt->_state) {
		case RESULTSET:
		case ATTR:
			return getNullNode();
		case AFFECTEDROWS:
		case COLUMN:
		case FIELD:
			return (!charstring::compare(name,"value"))?
						pvt->_attrnode:getNullNode();
		case COLUMNS:
		case ROWS:
		case ROW:
			return (!charstring::compare(name,"t"))?
						pvt->_attrnode:getNullNode();
		default:
			return getNullNode();
	}
}

domnode *sqlrresultsetdomnode::getAttributeIgnoringCase(
						const char *name) const {
	switch (pvt->_state) {
		case RESULTSET:
		case ATTR:
			return getNullNode();
		case AFFECTEDROWS:
		case COLUMN:
		case FIELD:
			return (!charstring::compareIgnoringCase(name,"value"))?
						pvt->_attrnode:getNullNode();
		case COLUMNS:
		case ROWS:
		case ROW:
			return (!charstring::compareIgnoringCase(name,"t"))?
						pvt->_attrnode:getNullNode();
		default:
			return getNullNode();
	}
}

domnode *sqlrresultsetdomnode::getAttribute(uint64_t position) const {
	switch (pvt->_state) {
		case RESULTSET:
		case ATTR:
			return getNullNode();
		case AFFECTEDROWS:
		case COLUMN:
		case FIELD:
		case COLUMNS:
		case ROWS:
		case ROW:
			return (!position)?pvt->_attrnode:getNullNode();
		default:
			return getNullNode();
	}
}

bool sqlrresultsetdomnode::isNullNode() const {
	return false;
}
