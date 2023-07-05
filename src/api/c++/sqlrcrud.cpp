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
	stringbuffer	columnsquery;
	columnsquery.append("select * from ");
	columnsquery.append(tbl);
	columnsquery.append(" where 1=0");
	if (!cur->getColumnList(tbl,NULL)) {
		return false;
	}
	for (uint32_t i=0; i<cur->colCount(); i++) {
		const char	*colname=cur->getColumnName(i);
		if (cur->getColumnIsPrimaryKey(i)) {
			setPrimaryKeyColumn(colname);
		}
		if (cur->getColumnIsAutoIncrement(i)) {
			setAutoIncrementColumn(colname);
		}
	}

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
			const char * const *values,
			const char * const *types) {

	// clear any previous error
	error.clear();

	stringbuffer	colstr;
	stringbuffer	valstr;
	const char	*bindformat=NULL;

	if (columns && values) {

		// build $(COLUMNS)
		bool		first=true;
		const char	*col;
		size_t		collen;
		for (const char * const *c=columns; *c; c++) {
			if (first) {
				first=false;
			} else {
				colstr.append(',');
			}
			getValidColumnName(*c,&col,&collen);
			colstr.append(col,collen);
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
					char	bf=bindformat[0];
					if (bf=='?') {
						valstr.append('?');
					} else if (bf=='$') {
						valstr.append('$')->append(col);
						col++;
					} else if (bf=='@' || bf==':') {
						valstr.append(bf)-> append(*c);
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
		bind(bindformat,columns,values,types);
	}

	// execute
	return cur->executeQuery();
}

bool sqlrcrud::doCreate(dictionary<const char *, const char *> *kvp) {

	// build columns/values
	linkedlist<const char *>	*keys=kvp->getKeys();
	uint64_t	keycount=keys->getCount();
	const char	**columns=new const char *[keycount+1];
	const char	**values=new const char *[keycount+1];
	const char	**types=new const char *[keycount+1];
	uint64_t	i=0;
	for (listnode<const char *> *node=keys->getFirst();
					node; node=node->getNext()) {
		columns[i]=node->getValue();
		values[i]=kvp->getValue(node->getValue());
		types[i]=deriveType(values[i]);
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;
	types[i]=NULL;

	// create
	bool	result=doCreate(columns,values,types);

	// clean up
	delete[] columns;
	delete[] values;
	delete[] types;

	return result;
}

bool sqlrcrud::doCreate(jsondom *j) {

	// build columns/values
	domnode		*datanode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("data");
	uint64_t	childcount=datanode->getChildCount();
	const char	**columns=new const char *[childcount+1];
	const char	**values=new const char *[childcount+1];
	const char	**types=new const char *[childcount+1];
	uint64_t	i=0;
	for (domnode *node=datanode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		columns[i]=node->getName();
		values[i]=node->getAttributeValue("v");
		types[i]=node->getAttributeValue("t");
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;
	types[i]=NULL;

	// create
	bool	result=doCreate(columns,values,types);

	// clean up
	delete[] columns;
	delete[] values;
	delete[] types;

	return result;
}

void sqlrcrud::getValidColumnName(const char *c,
					const char **col,
					size_t *collen) {

	// init return values...

	// col returns the actual start of the given column name
	*col=c;

	// collen returns the number of valid characters
	// after the start of the given column name
	*collen=0;

	// skip leading whitespace
	while (character::isWhitespace(*c)) {
		col++;
	}

	// run through the given column name
	for (;;) {

		// skip quoted strings
		if (*c=='\'') {
			while (*c && *c!='\'') {
				c++;
				(*collen)++;
			}
		} else if (*c=='"') {
			while (*c && *c!='"') {
				c++;
				(*collen)++;
			}
		} else if (*c=='`') {
			while (*c && *c!='`') {
				c++;
				(*collen)++;
			}
		}

		// bail if we encountered the end of the string
		if (!*c) {
			return;
		}

		// bail if we encounter an invalid character
		if (!(character::isAlphanumeric(*c) || *c=='_' || *c=='$')) {
			return;
		}

		// move on
		c++;
		(*collen)++;
	}
}

const char *sqlrcrud::deriveType(const char *value) {
	if (!value) {
		return "u";
	}
	if (charstring::isInteger(value)) {
		return "n";
	}
	if (!charstring::compareIgnoringCase(value,"t") ||
		!charstring::compareIgnoringCase(value,"true")) {
		return "t";
	}
	if (!charstring::compareIgnoringCase(value,"f") ||
		!charstring::compareIgnoringCase(value,"false")) {
		return "f";
	}
	return "s";
}

void sqlrcrud::bind(const char *bindformat,
				const char * const *columns,
				const char * const *values,
				const char * const *types) {

	if (charstring::isNullOrEmpty(bindformat)) {
		return;
	}

	const char * const *c=columns;
	const char * const *v=values;
	const char * const *t=types;
	char	bf=bindformat[0];
	m.clear();
	if (bf=='?'|| bf=='$') {
		uint64_t	i=1;
		while (*c && *t) {
			if (charstring::compareIgnoringCase(
							*c,autoinc) &&
				charstring::compareIgnoringCase(
							*c,primarykey)) {
				uint16_t	len=
						charstring::getIntegerLength(i);
				char		*b=(char *)m.allocate(len+1);
				charstring::printf(b,len,"%lld",i);
				if ((*t)[0]=='s') {
					cur->inputBind(b,*v);
				} else if ((*t)[0]=='n') {
					cur->inputBind(b,
						charstring::
							convertToInteger(*v));
				} else if ((*t)[0]=='t') {
					cur->inputBind(b,(int64_t)1);
				} else if ((*t)[0]=='f') {
					cur->inputBind(b,(int64_t)0);
				} else if ((*t)[0]=='u') {
					cur->inputBind(b,(const char *)NULL);
				}
				i++;
			}
			c++;
			v++;
			t++;
		}
	} else if (bf=='@' || bf==':') {
		while (*c && *t) {
			if (charstring::compareIgnoringCase(
							*c,autoinc) &&
				charstring::compareIgnoringCase(
							*c,primarykey)) {
				if ((*t)[0]=='s') {
					cur->inputBind(*c,*v);
				} else if ((*t)[0]=='n') {
					cur->inputBind(*c,
						charstring::
							convertToInteger(*v));
				} else if ((*t)[0]=='t') {
					cur->inputBind(*c,(int64_t)1);
				} else if ((*t)[0]=='f') {
					cur->inputBind(*c,(int64_t)0);
				} else if ((*t)[0]=='u') {
					cur->inputBind(*c,(const char *)NULL);
				}
			}
			c++;
			v++;
			t++;
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

	// read
	return doReadDelegate(wherestr.getString(),orderbystr.getString(),skip);
}

bool sqlrcrud::doRead(jsondom *j) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	domnode		*criterianode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("criteria")->
					getFirstTagChild();
	stringbuffer	wherestr;
	if (!buildJsonWhere(criterianode,&wherestr,
					readcontainspartialwhere)) {
		return false;
	}

	// build $(ORDERBY)
	domnode		*sortnode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("sort");
	stringbuffer	orderbystr;
	if (!buildJsonOrderBy(sortnode,&orderbystr,
					readcontainspartialorderby)) {
		return false;
	}

	// get skip
	domnode		*skipnode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("skip");
	uint64_t	skip=charstring::convertToInteger(
					skipnode->getAttributeValue("v"));

	// read
	return doReadDelegate(wherestr.getString(),orderbystr.getString(),skip);
}

bool sqlrcrud::doReadDelegate(const char *where,
				const char *orderby,
				uint64_t skip) {

	// prepare and execute
	cur->prepareQuery(readquery.getString());
	cur->substitution("WHERE",where);
	cur->substitution("ORDERBY",orderby);
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

	// handle degenerate case
	if (criteria->isNullNode()) {
		return true;
	}

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

	// handle degenerate case
	if (sort->isNullNode()) {
		return true;
	}

	// we might be at the root of the dom tree, descend
	if (sort->getType()==ROOT_DOMNODETYPE) {
		return buildJsonOrderBy(sort->getFirstTagChild(),
					orderbystr,containspartial);
	}

	bool		first=true;
	const char	*col;
	size_t		collen;
	for (domnode *node=sort->getFirstTagChild();
				!node->isNullNode();
				node=node->getNextTagSibling()) {

		if (first) {
			orderbystr->append((containspartial)?
						", ":" order by ");
			first=false;
		} else {
			orderbystr->append(", ");
		}
		getValidColumnName(node->getName(),&col,&collen);
		orderbystr->append(col,collen)->append(' ')->
				append(node->getAttributeValue("v"));
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
			const char * const *types,
			const char *criteria) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr,updatecontainspartialwhere)) {
		return false;
	}

	// update
	return doUpdateDelegate(columns,values,types,wherestr.getString());
}

bool sqlrcrud::doUpdate(dictionary<const char *, const char *> *kvp,
						const char *criteria) {

	// build columns/values
	linkedlist<const char *>	*keys=kvp->getKeys();
	uint64_t			keycount=keys->getCount();
	const char	**columns=new const char *[keycount+1];
	const char	**values=new const char *[keycount+1];
	const char	**types=new const char *[keycount+1];
	uint64_t	i=0;
	for (listnode<const char *> *node=keys->getFirst();
					node; node=node->getNext()) {
		columns[i]=node->getValue();
		values[i]=kvp->getValue(node->getValue());
		types[i]=deriveType(values[i]);
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;
	types[i]=NULL;

	// update
	bool	result=doUpdate(columns,values,types,criteria);

	// clean up
	delete[] columns;
	delete[] values;
	delete[] types;

	return result;
}

bool sqlrcrud::doUpdate(jsondom *j) {

	// clear any previous error
	error.clear();

	// build columns/values
	domnode		*datanode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("data");
	uint64_t	childcount=datanode->getChildCount();
	const char	**columns=new const char *[childcount+1];
	const char	**values=new const char *[childcount+1];
	const char	**types=new const char *[childcount+1];
	uint64_t	i=0;
	for (domnode *node=datanode->getFirstTagChild();
			!node->isNullNode(); node=node->getNextTagSibling()) {
		columns[i]=node->getName();
		values[i]=node->getAttributeValue("v");
		types[i]=node->getAttributeValue("t");
		i++;
	}
	columns[i]=NULL;
	values[i]=NULL;
	types[i]=NULL;

	// build $(WHERE)
	domnode		*criterianode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("criteria")->
					getFirstTagChild();
	stringbuffer	wherestr;
	if (!buildJsonWhere(criterianode,&wherestr,
					updatecontainspartialwhere)) {
		delete[] columns;
		delete[] values;
		delete[] types;
		return false;
	}

	// update
	bool	result=doUpdateDelegate(columns,values,types,
						wherestr.getString());

	// clean up
	delete[] columns;
	delete[] values;
	delete[] types;

	return result;
}

bool sqlrcrud::doUpdateDelegate(const char * const *columns,
				const char * const *values,
				const char * const *types,
				const char *where) {

	stringbuffer	setstr;
	const char	*bindformat=NULL;

	if (columns && values) {

		const char	*col;
		size_t		collen;

		// build $(SET)
		bindformat=con->bindFormat();
		if (!charstring::isNullOrEmpty(bindformat)) {
			char	bf=bindformat[0];
			if (bf=='?') {
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
					getValidColumnName(*c,&col,&collen);
					setstr.append(col,collen)->append('=');
					setstr.append('?');
				}
			} else if (bf=='$') {
				uint64_t	colind=1;
				for (const char * const *c=columns; *c; c++) {
					if (!charstring::compare(*c,autoinc)) {
						continue;
					}
					if (colind>1) {
						setstr.append(',');
					}
					getValidColumnName(*c,&col,&collen);
					setstr.append(col,collen)->append('=');
					setstr.append('$')->append(colind);
					colind++;
				}
			} else if (bf=='@' || bf==':') {
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
					getValidColumnName(*c,&col,&collen);
					setstr.append(col,collen)->append('=');
					setstr.append(bf);
					setstr.append(*c);
				}
			}
		}
	}

	// prepare
	cur->prepareQuery(updatequery.getString());

	// substitute
	cur->substitution("SET",setstr.getString());
	cur->substitution("WHERE",where);

	// bind
	if (columns && values) {
		bind(bindformat,columns,values,types);
	}

	// execute
	return cur->executeQuery();
}

bool sqlrcrud::doDelete(const char *criteria) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	stringbuffer	wherestr;
	if (!buildWhere(criteria,&wherestr,deletecontainspartialwhere)) {
		return false;
	}

	// delete
	return doDeleteDelegate(wherestr.getString());
}

bool sqlrcrud::doDelete(jsondom *j) {

	// clear any previous error
	error.clear();

	// build $(WHERE)
	domnode		*criterianode=j->getRootNode()->
					getFirstTagChild("r")->
					getFirstTagChild("criteria")->
					getFirstTagChild();
	stringbuffer	wherestr;
	if (!buildJsonWhere(criterianode,&wherestr,
					deletecontainspartialwhere)) {
		return false;
	}

	// delete
	return doDeleteDelegate(wherestr.getString());
}

bool sqlrcrud::doDeleteDelegate(const char *where) {

	// prepare and execute
	cur->prepareQuery(deletequery.getString());
	cur->substitution("WHERE",where);
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

scalarcollection<uint64_t> *sqlrcrud::getAffectedRowsScalar() {
	ars.clear();
	ars.setValue(getAffectedRows());
	return &ars;
}

listcollection<uint64_t> *sqlrcrud::getAffectedRowsList() {
	arl.clear();
	arl.append(getAffectedRows());
	return &arl;
}

dictionarycollection<const char *, uint64_t>
				*sqlrcrud::getAffectedRowsDictionary() {
	ard.clear();
	ard.setValue("r",getAffectedRows());
	return &ard;
}

tablecollection<uint64_t> *sqlrcrud::getAffectedRowsTable() {
	art.clear();
	art.setColumnName(0,"r");
	art.setValue(0,0,getAffectedRows());
	return &art;
}

scalarcollection<const char *> *sqlrcrud::getFirstFieldScalar() {
	ffs.setCursor(cur);
	return &ffs;
}

listcollection<const char *> *sqlrcrud::getFirstRowList() {
	frl.setCursor(cur);
	return &frl;
}

dictionarycollection<const char *,const char *>
					*sqlrcrud::getFirstRowDictionary() {
	frd.setCursor(cur);
	return &frd;
}

listcollection<const char *> *sqlrcrud::getFirstColumnList() {
	fcl.setCursor(cur);
	return &fcl;
}

tablecollection<const char *> *sqlrcrud::getResultSetTable() {
	rst.setCursor(cur);
	return &rst;
}
