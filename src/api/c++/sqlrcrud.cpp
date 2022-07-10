// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcrud.h>
#include <rudiments/memorypool.h>

sqlrcrud::sqlrcrud() {
	tbl=NULL;
	idsequence=NULL;
	primarykey=NULL;
	autoinc=NULL;
	readcontainspartialwhere=false;
	readcontainspartialorderby=false;
	updatecontainspartialwhere=false;
	deletecontainspartialwhere=false;
}

sqlrcrud::~sqlrcrud() {
	delete[] primarykey;
	delete[] tbl;
}

void sqlrcrud::setSqlrConnection(sqlrconnection *con) {
	this->con=con;
}

void sqlrcrud::setSqlrCursor(sqlrcursor *cur) {
	this->cur=cur;
}

void sqlrcrud::setTable(const char *tbl) {
	delete[] this->tbl;
	this->tbl=charstring::duplicate(tbl);

	if (charstring::isNullOrEmpty(idsequence)) {
		delete[] idsequence;
		charstring::printf(&idsequence,"%s_ids",tbl);
	}
}

void sqlrcrud::setIdSequence(const char *idsequence) {
	delete[] this->idsequence;
	this->idsequence=charstring::duplicate(idsequence);
}

void sqlrcrud::setPrimaryKeyColumn(const char *primarykey) {
	delete[] this->primarykey;
	this->primarykey=charstring::duplicate(primarykey);
}

void sqlrcrud::setAutoIncrementColumn(const char *autoinc) {
	delete[] this->autoinc;
	this->autoinc=charstring::duplicate(autoinc);
}

const char *sqlrcrud::getTable() {
	return tbl;
}

const char *sqlrcrud::getIdSequence() {
	return idsequence;
}

const char *sqlrcrud::getPrimaryKeyColumn() {
	return primarykey;
}

const char *sqlrcrud::getAutoIncrementColumn() {
	return autoinc;
}

bool sqlrcrud::buildQueries() {

	// bail if we don't have a connection, cursor, or table
	if (!con || !cur || !tbl) {
		return false;
	}

	// find the primary key and autoincrement column
	cur->lowerCaseColumnNames();
	if (!cur->getColumnList(tbl,NULL)) {
		return false;
	}
	for (uint64_t i=0; i<cur->rowCount(); i++) {

		// get the column name, "key", and "extra"
		const char	*colname=cur->getField(0,"column_name");
		const char	*colkey=cur->getField(0,"column_key");
		const char	*extra=cur->getField(0,"extra");

		// copy the primary key and autoincrement column
		// FIXME: this is valid for mysql, but maybe not for other dbs,
		// we need a standardized way of doing this, like
		// cur->getPrimaryKey() or something
		if (!charstring::compare(colkey,"PRI")) {
			setPrimaryKeyColumn(colname);
		}
		if (!charstring::compare(extra,"auto_increment")) {
			setAutoIncrementColumn(colname);
		}
	}
	// FIXME: set this back to whatever it was, which might not be mixed,
	// but there's currently no way to find out what it's currently set to
	cur->mixedCaseColumnNames();

	// build create (insert) query
	createquery.clear();
	createquery.append("insert into ")->append(tbl);
	createquery.append(" ($(COLUMNS)) values ($(VALUES))");

	// build read (select) query
	readquery.clear();
	readquery.append("select * from ")->append(tbl);
	readquery.append("$(WHERE)$(ORDERBY)");

	// build update query
	updatequery.clear();
	updatequery.append("update ")->append(tbl)->append(" set ");
	updatequery.append("$(SET)$(WHERE)");

	// build delete query
	deletequery.clear();
	deletequery.append("delete from ")->append(tbl)->append("$(WHERE)");

	// reset partial where flags
	readcontainspartialwhere=false;
	readcontainspartialorderby=false;
	updatecontainspartialwhere=false;
	deletecontainspartialwhere=false;

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

void sqlrcrud::setReadQueryContainsPartialWhere(bool containspartial) {
	readcontainspartialwhere=containspartial;
}

void sqlrcrud::setReadQueryContainsPartialOrderBy(bool containspartial) {
	readcontainspartialorderby=containspartial;
}

void sqlrcrud::setUpdateQueryContainsPartialWhere(bool containspartial) {
	updatecontainspartialwhere=containspartial;
}

void sqlrcrud::setDeleteQueryContainsPartialWhere(bool containspartial) {
	deletecontainspartialwhere=containspartial;
}

bool sqlrcrud::getReadQueryContainsPartialWhere() {
	return readcontainspartialwhere;
}

bool sqlrcrud::getReadQueryContainsPartialOrderBy() {
	return readcontainspartialorderby;
}

bool sqlrcrud::getUpdateQueryContainsPartialWhere() {
	return updatecontainspartialwhere;
}

bool sqlrcrud::getDeleteQueryContainsPartialWhere() {
	return deletecontainspartialwhere;
}

bool sqlrcrud::doCreate(const char * const *columns,
			const char * const *values) {

	// clear any previous error
	error.clear();

	stringbuffer	colstr;
	stringbuffer	valstr;
	const char	*bindformat=NULL;

	if (columns && values) {

		// build $(COLUMNS)
		bool	first=true;
		for (const char * const *c=columns; *c; c++) {
			if (first) {
				first=false;
			} else {
				colstr.append(',');
			}
			colstr.append(*c);
		}

		// build $(VALUES)
		bindformat=con->bindFormat();
		if (!charstring::isNullOrEmpty(bindformat)) {
			uint64_t	col=1;
			first=true;
			for (const char * const *c=columns; *c; c++) {
				if (first) {
					first=false;
				} else {
					valstr.append(',');
				}
				if (!charstring::compareIgnoringCase(
							*c,autoinc)) {
					valstr.append("null");
				} else if (!charstring::compareIgnoringCase(
							*c,primarykey)) {
					valstr.printf(
						con->nextvalFormat(),
						idsequence);
				} else {
					if (bindformat[0]=='?') {
						valstr.append('?');
					} else if (bindformat[0]=='$') {
						valstr.append('$')->append(col);
						col++;
					} else if (bindformat[0]=='@' ||
							bindformat[0]==':') {
						valstr.append(
							bindformat[0])->
								append(*c);
					}
				}
			}
		}
	}

	// prepare
	cur->prepareQuery(createquery.getString());


	// substitute
	cur->substitution("COLUMNS",colstr.getString());
	cur->substitution("VALUES",valstr.getString());

	// bind
	if (columns && values) {
		bind(bindformat,columns,values);
	}

	// execute
	return cur->executeQuery();
}

bool sqlrcrud::doCreate(dictionary<const char *, const char *> *kvp) {

	// decompose the dictionary
	linkedlist<const char *>	*keys=kvp->getKeys();
	const char	**columns=new const char *[keys->getLength()+1];
	const char	**values=new const char *[keys->getLength()+1];
	uint64_t	i=0;
	for (listnode<const char *> *node=keys->getFirst();
					node; node=node->getNext()) {
		columns[i]=node->getValue();
		values[i]=kvp->getValue(node->getValue());
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;

	// create
	bool	result=doCreate(columns,values);

	// clean up
	delete[] columns;
	delete[] values;

	return result;
}

void sqlrcrud::bind(const char *bindformat,
				const char * const *columns,
				const char * const *values) {

	if (charstring::isNullOrEmpty(bindformat)) {
		return;
	}

	// FIXME: there's no way to bind a NULL or non-string with this...

	const char * const *c=columns;
	const char * const *v=values;
	memorypool	m;
	if (bindformat[0]=='?'|| bindformat[0]=='$') {
		uint64_t	i=1;
		while (*v) {
			if (charstring::compareIgnoringCase(
							*c,autoinc) &&
				charstring::compareIgnoringCase(
							*c,primarykey)) {
				uint16_t	len=
						charstring::integerLength(i);
				char		*b=(char *)m.allocate(len+1);
				charstring::printf(b,len,"%lld",i);
				cur->inputBind(b,*v);
			}
			v++;
		}
	} else if (bindformat[0]=='@' || bindformat[0]==':') {
		while (*c && *v) {
			if (charstring::compareIgnoringCase(
							*c,autoinc) &&
				charstring::compareIgnoringCase(
							*c,primarykey)) {
				cur->inputBind(*c,*v);
			}
			c++;
			v++;
		}
	}
}

bool sqlrcrud::doRead(const char *criteria, const char *sort, uint64_t skip) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr,readcontainspartialwhere)) {
		return false;
	}

	// build $(ORDERBY)
	stringbuffer	orderbystr;
	if (!buildOrderBy(sort,&orderbystr,readcontainspartialorderby)) {
		return false;
	}

	// prepare and execute
	cur->prepareQuery(readquery.getString());
	cur->substitution("WHERE",wherestr.getString());
	cur->substitution("ORDERBY",orderbystr.getString());
	bool	success=cur->executeQuery();

	// skip to the "skip"th row
	cur->getField(skip,(uint32_t)0);

	return success;
}

bool sqlrcrud::buildWhere(const char *criteria,
				stringbuffer *wherestr,
				bool containspartial) {
	return buildClause(criteria,wherestr,true,containspartial);
}

bool sqlrcrud::buildOrderBy(const char *sort,
				stringbuffer *orderbystr,
				bool containspartial) {
	return buildClause(sort,orderbystr,false,containspartial);
}

bool sqlrcrud::buildClause(const char *domstr, stringbuffer *strb,
					bool where, bool containspartial) {

	if (!domstr) {
		return true;
	}

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
			error.append(j.getError());
			return false;
		}
		return (where)?
			buildJsonWhere(j.getRootNode(),strb,containspartial):
			buildJsonOrderBy(j.getRootNode(),strb,containspartial);
	} else

	// if it's XML
	if (*c=='<') {

		// parse the XML and build the where clause
		if (!x.parseString(c)) {
			error.append(x.getError());
			return false;
		}
		return (where)?
			buildXmlWhere(x.getRootNode(),strb,containspartial):
			buildXmlOrderBy(x.getRootNode(),strb,containspartial);
	}
	
	// unrecognized format
	error.append("unrecognized ")->
		append((where)?"where":"order by")->
		append(" clause format");
	return false;
}

bool sqlrcrud::buildJsonWhere(domnode *criteria,
				stringbuffer *wherestr,
				bool containspartial) {

	// criteria should be in jsonlogic format - http://jsonlogic.com

	// we might be at the root of the dom tree, or at the root json object,
	// in either case, descend
	if (criteria->getType()==ROOT_DOMNODETYPE ||
		!charstring::compare(criteria->getName(),"r")) {
		return buildJsonWhere(criteria->getFirstTagChild(),
						wherestr,containspartial);
	}

	// this node should be a var or operation

	// get the op
	const char	*op=criteria->getName();

	// handle variables
	if (!charstring::compare(op,"var")) {

		// the top-level can't be a variable
		if (!wherestr->getSize()) {
			error.append("in criteria, top-level JSON object "
							"must be an operation");
			return false;
		}

		// append the variable name
		wherestr->append(criteria->getAttributeValue("v"));
		return true;
	}

	// handle operations...

	// it should be an array
	if (charstring::compare(criteria->getAttributeValue("t"),"a")) {
		error.append("in criteria, array expected");
		return false;
	}

	// run through the child nodes...
	bool	first=true;
	for (domnode *node=criteria->getFirstTagChild();
					!node->isNullNode();
					node=node->getNextTagSibling()) {

		if (first) {

			// begin the where clause
			if (!wherestr->getSize()) {
				wherestr->append((containspartial)?
							" and ":" where ");
			}

			// begin the group...
			wherestr->append('(');
		}

		// get array member type
		const char	*t=node->getAttributeValue("t");
		const char	*v=node->getAttributeValue("v");

		switch (t[0]) {

			// string literals
			case 's':
				wherestr->append('\'')->append(v)->append('\'');
				break;

			// numeric literals
			case 'n':
				wherestr->append(v);
				break;

			// true
			case 't':
				wherestr->append("true");
				break;

			// false
			case 'f':
				wherestr->append("false");
				break;

			// null
			case 'u':
				wherestr->append("null");
				break;

			// either a var or nested operation
			case 'o':
				if (!buildJsonWhere(
						node->getFirstTagChild(),
						wherestr,containspartial)) {
					return false;
				}
				break;

			// unexpected node encountered
			default:
				error.append("in criteria, unexpected "
						"json dom node encountered");
				return false;
		}

		// special handling for isnull and isnotnull
		if (!charstring::compare(op,"isnull")) {
			wherestr->append(" is null)");
			break;
		}
		if (!charstring::compare(op,"isnotnull")) {
			wherestr->append(" is not null)");
			break;
		}

		// are we on the last member?
		bool	last=node->getNextTagSibling()->isNullNode();

		// special handling for in
		if (!charstring::compare(op,"in")) {
			if (first) {
				wherestr->append(" in (");
			} else if (last) {
				wherestr->append("))");
			}
		} else {
			if (last) {
				// end the group...
				wherestr->append(')');
			} else {
				// append the operator between members
				wherestr->append(' ')->append(op)->append(' ');
			}
		}

		first=false;
	}

	return true;
}

bool sqlrcrud::buildXmlWhere(domnode *criteria,
				stringbuffer *wherestr,
				bool containspartial) {
	// FIXME: implement this...
	return false;
}

bool sqlrcrud::buildJsonOrderBy(domnode *sort,
				stringbuffer *orderbystr,
				bool containspartial) {

	// sort should be something like:
	// <r t="o">
	//   <col1 t="s" v="asc"/>
	//   <col2 t="s" v="asc"/>
	//   <col3 t="s" v="desc"/>
	// </r>

	// we might be at the root of the dom tree, descend
	if (sort->getType()==ROOT_DOMNODETYPE) {
		return buildJsonOrderBy(sort->getFirstTagChild(),
					orderbystr,containspartial);
	}

	bool	first=true;
	for (domnode *node=sort->getFirstTagChild();
				!node->isNullNode();
				node=node->getNextTagSibling()) {

		const char	*var=node->getName();
		const char	*order=node->getAttributeValue("v");
		// FIXME: validate var
		if (first) {
			orderbystr->append((containspartial)?
						", ":" order by ");
			first=false;
		} else {
			orderbystr->append(", ");
		}
		orderbystr->append(var)->append(' ')->append(order);
	}
	return true;
}

bool sqlrcrud::buildXmlOrderBy(domnode *sort,
				stringbuffer *orderbystr,
				bool containspartial) {
	// FIXME: implement this...
	return false;
}

bool sqlrcrud::doUpdate(const char * const *columns,
			const char * const *values,
			const char *criteria) {

	// clear any previous error
	error.clear();

	stringbuffer	setstr;
	const char	*bindformat=NULL;

	if (columns && values) {

		// build $(SET)
		bindformat=con->bindFormat();
		if (!charstring::isNullOrEmpty(bindformat)) {
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
		}
	}

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr,updatecontainspartialwhere)) {
		return false;
	}

	// prepare
	cur->prepareQuery(updatequery.getString());

	// substitute
	cur->substitution("SET",setstr.getString());
	cur->substitution("WHERE",wherestr.getString());

	// bind
	if (columns && values) {
		bind(bindformat,columns,values);
	}

	// execute
	return cur->executeQuery();
}

bool sqlrcrud::doUpdate(dictionary<const char *, const char *> *kvp,
						const char *criteria) {

	// decompose the dictionary
	linkedlist<const char *>	*keys=kvp->getKeys();
	const char	**columns=new const char *[keys->getLength()+1];
	const char	**values=new const char *[keys->getLength()+1];
	uint64_t	i=0;
	for (listnode<const char *> *node=keys->getFirst();
					node; node=node->getNext()) {
		columns[i]=node->getValue();
		values[i]=kvp->getValue(node->getValue());
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;

	// update
	bool	result=doUpdate(columns,values,criteria);

	// clean up
	delete[] columns;
	delete[] values;

	return result;
}

bool sqlrcrud::doDelete(const char *criteria) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr,deletecontainspartialwhere)) {
		return false;
	}

	// prepare and execute
	cur->prepareQuery(deletequery.getString());
	cur->substitution("WHERE",wherestr.getString());
	return cur->executeQuery();
}

const char *sqlrcrud::getErrorMessage() {
	if (error.getSize()) {
		return error.getString();
	}
	return cur->errorMessage();
}

int64_t sqlrcrud::getErrorCode() {
	if (error.getSize()) {
		return 0;
	}
	return cur->errorNumber();
}

uint64_t sqlrcrud::getAffectedRows() {
	return cur->affectedRows();
}

const scalarcollection<uint64_t> *sqlrcrud::getAffectedRowsScalar() {
	ars.clear();
	ars.setValue(getAffectedRows());
	return &ars;
}

const listcollection<uint64_t> *sqlrcrud::getAffectedRowsList() {
	arl.clear();
	arl.append(getAffectedRows());
	return &arl;
}

const dictionarycollection<const char *, uint64_t>
				*sqlrcrud::getAffectedRowsDictionary() {
	ard.clear();
	ard.setValue("r",getAffectedRows());
	return &ard;
}

const tablecollection<uint64_t> *sqlrcrud::getAffectedRowsTable() {
	art.clear();
	art.setColumnName(0,"r");
	art.setValue(0,0,getAffectedRows());
	return &art;
}

const scalarcollection<const char *> *sqlrcrud::getFirstFieldScalar() {
	ffs.setCursor(cur);
	return &ffs;
}

const listcollection<const char *> *sqlrcrud::getFirstRowList() {
	frl.setCursor(cur);
	return &frl;
}

const dictionarycollection<const char *,const char *>
					*sqlrcrud::getFirstRowDictionary() {
	frd.setCursor(cur);
	return &frd;
}

const listcollection<const char *> *sqlrcrud::getFirstColumnList() {
	fcl.setCursor(cur);
	return &fcl;
}

const tablecollection<const char *> *sqlrcrud::getResultSetTable() {
	rst.setCursor(cur);
	return &rst;
}
