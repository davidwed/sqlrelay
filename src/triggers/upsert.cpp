// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>
#include <rudiments/regularexpression.h>
#include <rudiments/linkedlist.h>
#include <rudiments/error.h>
#include <rudiments/snooze.h>

#define NEED_IS_BIND_DELIMITER 1
#include <bindvariables.h>

class SQLRSERVER_DLLSPEC sqlrtrigger_upsert : public sqlrtrigger {
	public:
		sqlrtrigger_upsert(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters);

		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success);
	private:
		bool	errorEncountered(sqlrservercursor *sqlrcur);
		domnode	*tableEncountered(const char *table);
		bool	insertToUpdate(const char *table,
					char **cols,
					uint64_t colcount,
					const char *autoinccolumn,
					const char *values,
					domnode *tablenode,
					stringbuffer *query);
		bool	isBind(const char *var);
		bool	copyInputBinds(sqlrservercursor *cursqlrcur,
					sqlrservercursor *newsqlrcur);
		void	copyInputBind(memorypool *pool,
					bool where,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source);
		void	deleteCols(char **cols, uint64_t colcount);

		sqlrservercontroller	*cont;

		bool	debug;

		domnode	*errors;
		domnode	*tables;

		bool	disabled;

		dictionary<const char *, const char *>	wherebinds;
};

sqlrtrigger_upsert::sqlrtrigger_upsert(sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	this->cont=cont;

	debug=cont->getConfig()->getDebugTriggers();

	errors=parameters->getFirstTagChild("errors");
	tables=parameters->getFirstTagChild("tables");
}

bool sqlrtrigger_upsert::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success) {

	// in the before phase, don't do anything
	if (before) {
		return *success;
	}

	// in the after phase...

	// get the query and query type
	//
	// NOTE: for now queryType() groups simple insert, multi-insert,
	// insert/select and select-into into SQLRQUERYTYPE_INSERT
	const char		*query=sqlrcur->getQueryBuffer();
	uint32_t		querylen=sqlrcur->getQueryLength();
	sqlrquerytype_t		querytype=sqlrcur->queryType(query,querylen);

	// bail if the query wasn't an insert, or if the insert didn't
	// encounter an error that should trigger an update
	if (querytype!=SQLRQUERYTYPE_INSERT || !errorEncountered(sqlrcur)) {
		return *success;
	}

	// parse the query
	// NOTE: parseInsert will populate querytype with a more specific value
	char		*table=NULL;
	char		**cols=NULL;
	uint64_t	colcount=0;
	const char	*values=NULL;
	const char	*autoinccolumn=NULL;
	cont->parseInsert(query,querylen,&querytype,
				&table,&cols,&colcount,
				NULL,&autoinccolumn,NULL,&values);

	// bail if the query wasn't a simple insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		deleteCols(cols,colcount);
		delete[] table;
		return *success;
	}

	// bail if the table isn't one that we want to update
	domnode	*tablenode=tableEncountered(table);
	if (!tablenode) {
		deleteCols(cols,colcount);
		delete[] table;
		return *success;
	}

	// Declare a buffer to store the update query and open a new cursor
	// to run the update.  We don't want to just use the current cursor
	// because the client may want to reexecute the insert.
	stringbuffer		update;
	sqlrservercursor	*newsqlrcur=cont->newCursor();

	// convert the insert to an update,
	// copy input binds from sqlrcur to newsqlrcur,
	// prepare and execute the query
	*success=insertToUpdate(table,cols,colcount,
				autoinccolumn,
				values,tablenode,&update) &&
		copyInputBinds(sqlrcur,newsqlrcur) &&
		cont->prepareQuery(newsqlrcur,
				update.getString(),
				update.getStringLength()) &&
		cont->executeQuery(newsqlrcur);

	// FIXME: copy errors and affected rows to sqlrcur

	// clean up
	cont->closeResultSet(newsqlrcur);
	cont->close(newsqlrcur);
	cont->deleteCursor(newsqlrcur);
	deleteCols(cols,colcount);
	delete[] table;
	return *success;
}

bool sqlrtrigger_upsert::errorEncountered(sqlrservercursor *sqlrcur) {

	// look through the errors and see if we find
	// one that matches the sqlrcur's error
	// FIXME: maybe cache the number using
	// domnode::setData() for future lookups
	for (domnode *node=errors->getFirstTagChild("error");
				!node->isNullNode();
				node=node->getNextTagSibling("error")) {
		const char	*string=node->getAttributeValue("string");
		const char	*number=node->getAttributeValue("number");
		if ((string && charstring::contains(
					sqlrcur->getErrorBuffer(),string)) ||
			(number && sqlrcur->getErrorNumber()==
					charstring::toInteger(number))) {
			return true;
		}
	}
	return false;
}

domnode *sqlrtrigger_upsert::tableEncountered(const char *table) {

	// look through the tables and see if we find one that matches "table"
	for (domnode *node=tables->getFirstTagChild("table");
				!node->isNullNode();
				node=node->getNextTagSibling("table")) {
		if (!charstring::compare(
				node->getAttributeValue("name"),table)) {
			return node;
		}
	}
	return NULL;
}

bool sqlrtrigger_upsert::insertToUpdate(const char *table,
					char **cols,
					uint64_t colcount,
					const char *autoinccolumn,
					const char *values,
					domnode *tablenode,
					stringbuffer *query) {

	// split values
	char		**vals;
	uint64_t	valscount;
	// FIXME: use a split that considers quoting
	charstring::split(values,",",false,&vals,&valscount);

	// begin building the update query
	query->append("update ")->append(table)->append(" set ");

	// build the set clause and map column names to values
	dictionary<const char *,const char *>	valuemap;
	for (uint64_t i=0; i<colcount; i++) {

		// FIXME: ignore the primary key

		// ignore any auto-increment columns
		if (!charstring::compare(cols[i],autoinccolumn)) {
			continue;
		}

		// get the column/value pair
		const char	*col=cols[i];
		const char	*val=vals[i];

		// append them to the set clause
		if (i) {
			query->append(',');
		}
		query->append(col)->append('=')->append(val);

		// map column -> value for use in the where clause later
		valuemap.setValue(col,val);
	}

	// begin building the where clause
	query->append(" where ");

	// build the where clause
	bool	retval=true;
	bool	first=true;
	for (domnode *node=tablenode->getFirstTagChild("column");
				!node->isNullNode();
				node=node->getNextTagSibling("column")) {

		// get the column/value pair
		const char	*col=node->getAttributeValue("name");
		const char	*val;
		if (!valuemap.getValue(col,&val)) {
			// bail if we didn't find a value for this column
			// FIXME: maybe we should set an error here, if we
			// don't then the original error that the insert failed
			// with will still be the error, and the fact that it
			// didn't contain a column that we need to perform the
			// update won't be immediately obvious
			retval=false;
		}

		// append them to the where clause
		if (!first) {
			query->append(" and ");
		}
		query->append(col)->append('=');

		if (isBind(val)) {
			// if val is a bind then use the
			// where-clause version of it
			// FIXME: does the bind name include the delimiter?
			query->append(wherebinds.getValue(val));
		} else {
			query->append(val);
		}
		first=false;
	}

	// clean up
	for (uint64_t i=0; i<valscount; i++) {
		delete[] vals[i];
	}
	delete[] vals;
	
	return retval;
}

bool sqlrtrigger_upsert::isBind(const char *var) {
	return var && isBindDelimiter(var,
				cont->getConfig()->
				getBindVariableDelimiterQuestionMarkSupported(),
				cont->getConfig()->
				getBindVariableDelimiterColonSupported(),
				cont->getConfig()->
				getBindVariableDelimiterAtSignSupported(),
				cont->getConfig()->
				getBindVariableDelimiterDollarSignSupported());
}

bool sqlrtrigger_upsert::copyInputBinds(sqlrservercursor *cursqlrcur,
					sqlrservercursor *newsqlrcur) {

	// make 2 copies of cursqlrcur's input binds in newsqlrcur:
	// * one of each bind to use in the set clause
	// * one of each bind to use in the where clause

	// count cursqlrcur's binds and make sure we
	// can allocate twice as many in newsqlrcur
	uint16_t	incount=cursqlrcur->getInputBindCount();
	if (incount*2>cont->getConfig()->getMaxBindCount()) {
		// FIXME: set an error, like too many binds or something
		return false;
	}

	// copy the input binds, making two copies of each in newsqlrcur
	memorypool		*newsqlrcurpool=cont->getBindPool(newsqlrcur);
	sqlrserverbindvar	*curinvars=cursqlrcur->getInputBinds();
	sqlrserverbindvar	*newinvars=newsqlrcur->getInputBinds();
	for (uint16_t i=0; i<incount; i++) {
		copyInputBind(newsqlrcurpool,
				false,&(newinvars[i]),&(curinvars[i]));
		copyInputBind(newsqlrcurpool,
				true,&(newinvars[incount+i]),&(curinvars[i]));
	}

	// set newsqlrcur's input bind count
	newsqlrcur->setInputBindCount(incount*2);

	return true;
}

void sqlrtrigger_upsert::copyInputBind(memorypool *pool, bool where,
						sqlrserverbindvar *dest,
						sqlrserverbindvar *source) {

	// byte-copy everything
	bytestring::copy(dest,source,sizeof(sqlrserverbindvar));

	// The shallow-copy above will aim the variable, value.stringval,
	// and value.dateval.buffer pointers to the strings stored in the
	// main cursor's memorypool.  There's no need to make a copy of
	// those strings, as they will persist for as long as these binds do.

	// So, for the copy of the bind that we'll use in the set clause,
	// we can bail here.
	if (!where) {
		return;
	}

	// We do need to rename the variable for the copy of the bind that
	// we'll use in the where clause though....

	// prepend "where_" to the variable name
	// FIXME: not all db's support named binds
	// FIXME: does "variable" include the delimiter?
	dest->variablesize+=6;
	dest->variable=(char *)pool->allocate(6+dest->variablesize+1);
	charstring::copy(dest->variable,"where_");
	charstring::append(dest->variable,source->variable);

	// map the source -> dest variable name for
	// easier lookup when building the update query
	wherebinds.setValue(source->variable,dest->variable);
}

void sqlrtrigger_upsert::deleteCols(char **cols, uint64_t colcount) {
	for (uint64_t i=0; i<colcount; i++) {
		delete[] cols[i];
	}
	delete[] cols;
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_upsert(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_upsert(cont,ts,parameters);
	}
}
