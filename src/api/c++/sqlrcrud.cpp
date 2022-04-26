// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcrud.h>

sqlrcrud::sqlrcrud() {
	table=NULL;
	primarykey=NULL;
	autoinc=NULL;
	columns=NULL;
}

sqlrcrud::~sqlrcrud() {
	delete[] primarykey;
	delete[] table;
	deleteColumns();
}

void sqlrcrud::deleteColumns() {
	if (columns) {
		for (char **c=columns; *c; c++) {
			delete[] *c;
		}
		delete[] columns;
	}
}

uint64_t sqlrcrud::countColumns(const char * const *columns) {
	uint64_t	count=0;
	if (columns) {
		for (const char * const *c=columns; *c; c++) {
			count++;
		}
	}
	return count;
}

void sqlrcrud::copyColumns(const char * const *columns) {
	uint64_t	count=countColumns(columns);
	this->columns=new char *[count+1];
	count=0;
	if (columns) {
		for (const char * const *c=columns; *c; c++) {
			this->columns[count]=charstring::duplicate(*c);
			count++;
		}
	}
	this->columns[count]=NULL;
}

void sqlrcrud::setSqlrConnection(sqlrconnection *con) {
	this->con=con;
}

void sqlrcrud::setSqlrCursor(sqlrcursor *cur) {
	this->cur=cur;
}

void sqlrcrud::setTable(const char *table) {
	delete[] this->table;
	this->table=charstring::duplicate(table);
}

void sqlrcrud::setPrimaryKeyColumn(const char *primarykey) {
	delete[] this->primarykey;
	this->primarykey=charstring::duplicate(primarykey);
}

void sqlrcrud::setAutoIncrementColumn(const char *autoinc) {
	delete[] this->autoinc;
	this->autoinc=charstring::duplicate(autoinc);
}

void sqlrcrud::setColumns(const char * const *columns) {
	deleteColumns();
	copyColumns(columns);
}

const char *sqlrcrud::getTable() {
	return table;
}

const char *sqlrcrud::getPrimaryKeyColumn() {
	return primarykey;
}

const char *sqlrcrud::getAutoIncrementColumn() {
	return autoinc;
}

const char * const *sqlrcrud::getColumns() {
	return columns;
}

bool sqlrcrud::buildQueries() {

	// bail if we don't have a connection, cursor, or table
	if (!con || !cur || !table) {
		return false;
	}

	// get the columns for this table
	delete[] primarykey;
	deleteColumns();
	cur->lowerCaseColumnNames();
	if (!cur->getColumnList(table,NULL)) {
		return false;
	}
	columns=new char *[cur->rowCount()+1];
	for (uint64_t i=0; i<cur->rowCount(); i++) {

		// get the column name and "key"
		const char	*colname=cur->getField(0,"column_name");
		const char	*colkey=cur->getField(0,"column_key");

		// copy the primary key
		// FIXME: this is valid for mysql, but maybe not for other dbs,
		// we need a standardized way of doing this, like
		// cur->getPrimaryKey() or something
		if (!charstring::compare(colkey,"PRI")) {
			primarykey=charstring::duplicate(colname);
		}

		// copy the column name
		columns[i]=charstring::duplicate(colname);
	}
	columns[cur->rowCount()]=NULL;
	// FIXME: set this back to whatever it was, which might not be mixed,
	// but there's currently no way to find out what it's currently set to
	cur->mixedCaseColumnNames();

	// build create (insert) query
	createquery.clear();
	createquery.append("insert into ")->append(table);
	createquery.append(" ($(COLUMNS)) values ($(VALUES))");

	// build read (select) query
	readquery.clear();
	readquery.append("select * from ")->append(table);
	readquery.append("$(WHERE)$(ORDERBY)");

	// build update query
	updatequery.clear();
	updatequery.append("update ")->append(table)->append(" set ");
	updatequery.append("$(SET)$(WHERE)");

	// build delete query
	deletequery.clear();
	deletequery.append("delete from ")->append(table)->append("$(WHERE)");

	return true;
}

void sqlrcrud::setCreateQuery(const char *createquery) {
	this->createquery.clear();
	this->createquery.append(createquery);
}

void sqlrcrud::setReadQuery(const char *readquery) {
	this->readquery.clear();
	this->readquery.append(readquery);
}

void sqlrcrud::setUpdateQuery(const char *updatequery) {
	this->updatequery.clear();
	this->updatequery.append(updatequery);
}

void sqlrcrud::setDeleteQuery(const char *deletequery) {
	this->deletequery.clear();
	this->deletequery.append(deletequery);
}

const char *sqlrcrud::getCreateQuery() {
	return createquery.getString();
}

const char *sqlrcrud::getReadQuery() {
	return readquery.getString();
}

const char *sqlrcrud::getUpdateQuery() {
	return updatequery.getString();
}

const char *sqlrcrud::getDeleteQuery() {
	return deletequery.getString();
}

bool sqlrcrud::doCreate(const char * const *columns,
			const char * const *values) {

	// build $(COLUMNS)
	stringbuffer	colstr;
	bool	first=true;
	for (const char * const *c=columns; *c; c++) {
		if (!charstring::compare(*c,autoinc)) {
			continue;
		}
		if (first) {
			first=false;
		} else {
			colstr.append(',');
		}
		colstr.append(*c);
	}

	// build $(VALUES)
	stringbuffer	valstr;
	const char	*bindformat=con->bindFormat();
	if (bindformat[0]=='?') {
		first=true;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (first) {
				first=false;
			} else {
				valstr.append(',');
			}
			valstr.append('?');
		}
	} else if (bindformat[0]=='$') {
		uint64_t	col=1;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (col>1) {
				valstr.append(',');
			}
			valstr.append('$')->append(col);
			col++;
		}
	} else if (bindformat[0]=='@' || bindformat[0]==':') {
		first=true;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (first) {
				first=false;
			} else {
				valstr.append(',');
			}
			valstr.append(bindformat[0]);
			valstr.append(*c);
		}
	}

	// prepare and execute
	cur->prepareQuery(createquery.getString());
	cur->substitution("COLUMNS",colstr.getString());
	cur->substitution("VALUES",valstr.getString());
	return cur->executeQuery();
}

bool sqlrcrud::doRead(const char *criteria, const char *sort,
				uint64_t skip, uint64_t fetch) {

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr)) {
		// FIXME: set error
		return false;
	}

	// build $(ORDERBY)
	stringbuffer	orderbystr;
	if (!buildOrderBy(sort,&orderbystr)) {
		// FIXME: set error
		return false;
	}

	// FIXME: implement skip/fetch

	// prepare and execute
	cur->prepareQuery(readquery.getString());
	cur->substitution("WHERE",wherestr.getString());
	cur->substitution("ORDERBY",orderbystr.getString());
	return cur->executeQuery();
}

bool sqlrcrud::buildWhere(const char *criteria, stringbuffer *wherestr) {
	return buildClause(criteria,wherestr,true);
}

bool sqlrcrud::buildOrderBy(const char *sort, stringbuffer *orderbystr) {
	return buildClause(sort,orderbystr,false);
}

bool sqlrcrud::buildClause(const char *domstr, stringbuffer *strb, bool where) {

	// domstr should be an XML or JSON string, parse it...

	// skip whitespace
	const char *c=domstr;
	while (character::isWhitespace(*c)) {
		 c++;
	}
	if (!c) {
		return true;
	}

	// if it's JSON...
	if (*c=='{') {

		// parse the JSON and build the where clause
		if (!j.parseString(c)) {
			// FIXME: set error
			return false;
		}
		return (where)?buildJSONWhere(j.getRootNode(),strb):
				buildJSONOrderBy(j.getRootNode(),strb);
	} else

	// if it's XML
	if (*c=='<') {

		// parse the XML and build the where clause
		if (!x.parseString(c)) {
			// FIXME: set error
			return false;
		}
		return (where)?buildXMLWhere(x.getRootNode(),strb):
				buildXMLOrderBy(x.getRootNode(),strb);
	}
	
	// FIXME: set error
	return false;
}

bool sqlrcrud::buildJSONWhere(domnode *critera, stringbuffer *wherestr) {

	// critera should be something like:
	// <field t="s" v="col1"/>
	// <operator t="s" v="="/>
	// <value t="s" v="val1"/>
	// <boolean t="s" v="and"/>
	// <field t="s" v="col2"/>
	// <operator t="s" v="in"/>
	// <list t="a">
	//   <v t="s" v="val1"/>
	//   <v t="s" v="val2"/>
	//   <v t="s" v="val3"/>
	// </list>
	// <boolean t="s" v="and"/>
	// <group t="o">
	// ...
	// </group>

	// FIXME: implement this...
	return true;
}

bool sqlrcrud::buildXMLWhere(domnode *critera, stringbuffer *wherestr) {
	// FIXME: implement this...
	return true;
}

bool sqlrcrud::buildJSONOrderBy(domnode *sort, stringbuffer *orderbystr) {
	// FIXME: implement this...
	return true;
}

bool sqlrcrud::buildXMLOrderBy(domnode *sort, stringbuffer *orderbystr) {
	// FIXME: implement this...
	return true;
}

bool sqlrcrud::doUpdate(const char * const *columns,
			const char * const *values,
			const char *criteria) {

	// build $(SET)
	stringbuffer	setstr;
	const char	*bindformat=con->bindFormat();
	if (bindformat[0]=='?') {
		bool	first=true;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (first) {
				first=false;
			} else {
				setstr.append(',');
			}
			setstr.append(*c)->append('=');
			setstr.append('?');
		}
	} else if (bindformat[0]=='$') {
		uint64_t	col=1;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (col>1) {
				setstr.append(',');
			}
			setstr.append(*c)->append('=');
			setstr.append('$')->append(col);
			col++;
		}
	} else if (bindformat[0]=='@' || bindformat[0]==':') {
		bool	first=true;
		for (const char * const *c=columns; *c; c++) {
			if (!charstring::compare(*c,autoinc)) {
				continue;
			}
			if (first) {
				first=false;
			} else {
				setstr.append(',');
			}
			setstr.append(*c)->append('=');
			setstr.append(bindformat[0]);
			setstr.append(*c);
		}
	}

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr)) {
		// FIXME: set error
		return false;
	}

	// prepare and execute
	cur->prepareQuery(updatequery.getString());
	cur->substitution("SET",setstr.getString());
	cur->substitution("WHERE",wherestr.getString());
	return cur->executeQuery();
}

bool sqlrcrud::doDelete(const char *criteria) {

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr)) {
		// FIXME: set error
		return false;
	}

	// prepare and execute
	cur->prepareQuery(deletequery.getString());
	cur->substitution("WHERE",wherestr.getString());
	return cur->executeQuery();
}

const char *sqlrcrud::getErrorMessage() {
	return cur->errorMessage();
}

int64_t sqlrcrud::getErrorCode() {
	return cur->errorNumber();
}

uint64_t sqlrcrud::getAffectedRows() {
	return cur->affectedRows();
}

sqlrscalar *sqlrcrud::getScalar() {
	scl.setCursor(cur);
	return &scl;
}

sqlrrowlinkedlist *sqlrcrud::getRowLinkedList() {
	rlst.setCursor(cur);
	return &rlst;
}

sqlrrowdictionary *sqlrcrud::getRowDictionary() {
	rdct.setCursor(cur);
	return &rdct;
}

sqlrresultsetlinkedlist *sqlrcrud::getResultSetLinkedList() {
	rslst.setCursor(cur);
	return &rslst;
}

sqlrresultsettable *sqlrcrud::getResultSetTable() {
	rstbl.setCursor(cur);
	return &rstbl;
}
