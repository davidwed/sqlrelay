// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>
#include <rudiments/regularexpression.h>
#include <rudiments/linkedlist.h>
#include <rudiments/error.h>
#include <rudiments/snooze.h>

#define NEED_IS_BIND_DELIMITER 1
#include <bindvariables.h>
#include <defines.h>

class SQLRSERVER_DLLSPEC sqlrtrigger_upsert : public sqlrtrigger {
	public:
		sqlrtrigger_upsert(sqlrservercontroller *cont,
						domnode *parameters);

		bool	runAfter(sqlrserverconnection *sqlrcon,
						sqlrservercursor *icur);
	private:
		bool	errorEncountered(sqlrservercursor *icur);
		domnode	*tableEncountered(const char *table);
		bool	copyInputBinds(sqlrservercursor *ucur,
					sqlrservercursor *icur,
					linkedlist<char *> *cols,
					linkedlist<char *> *vals,
					domnode *tablenode);
		void	copyInputBind(memorypool *pool,
					bool where,
					sqlrserverbindvar *ubind,
					sqlrserverbindvar *ibind,
					uint16_t bindnumber);
		bool	convertInsertToUpdate(
					sqlrservercursor *ucur,
					const char *table,
					linkedlist<char *> *cols,
					linkedlist<char *> *vals,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					domnode *tablenode,
					stringbuffer *query);
		bool	isBind(const char *var);

		domnode	*errors;
		domnode	*tables;

		dictionary<const char *, const char *>	settowhere;
};

sqlrtrigger_upsert::sqlrtrigger_upsert(sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrtrigger(cont,parameters) {
	errors=parameters->getFirstTagChild("errors");
	tables=parameters->getFirstTagChild("tables");
}

bool sqlrtrigger_upsert::runAfter(sqlrserverconnection *sqlrcon,
						sqlrservercursor *icur) {

	// bail if the query was suppressed
	if (cont->getQuerySuppressed(icur)) {
		return true;
	}

	// after the query has been run...

	// get the query and query type
	// NOTE: for now determineQueryType() groups simple insert,
	// multi-insert, insert/select and select-into into SQLRQUERYTYPE_INSERT
	const char		*query=cont->getQueryBuffer(icur);
	uint32_t		querylen=cont->getQuerySize(icur);
	sqlrquerytype_t		querytype=icur->getQueryType();

	debugStart("upsert");
	debugWrite("triggering query:\n%.*s",querylen,query);
	debugWrite("query type: %d",querytype);

	// bail if the query wasn't an insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		debugWrite("query was not an insert");
		debugEnd();
		return true;
	}

	// bail if the query didn't throw an error that we care about
	if (!errorEncountered(icur)) {
		debugWrite("no matching error found for:");
		debugWrite("%d: %.*s",cont->getErrorNumber(icur),
					cont->getErrorSize(icur),
					cont->getErrorBuffer(icur));
		debugEnd();
		return true;
	}

	// parse the query
	// NOTE: parseInsert will populate querytype with a more specific value
	char			*table=NULL;
	linkedlist<char *>	*cols=NULL;
	const char		*autoinccolumn=NULL;
	const char		*primarykeycolumn=NULL;
	linkedlist<char *>	*vals=NULL;
	cont->parseInsert(query,querylen,&querytype,
				&table,&cols,NULL,
				&autoinccolumn,NULL,
				&primarykeycolumn,NULL,
				&vals,NULL);

	// bail if the query wasn't a simple insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		debugWrite("query was not a simple insert");
		debugEnd();
		delete[] table;
		delete cols;
		delete vals;
		return true;
	}

	// debug
	debugWrite("table: %s",table);

	// bail if the table isn't one that we care about
	domnode	*tablenode=tableEncountered(table);
	if (!tablenode) {
		debugWrite("table not configured for upsert");
		debugEnd();
		delete[] table;
		delete cols;
		delete vals;
		return true;
	}

	// debug
	if (getDebug()) {
		debugStart("columns");
		if (cols) {
			for (listnode<char *> *node=cols->getFirst();
						node; node=node->getNext()) {
				debugWrite("%s",node->getValue());
			}
			debugWrite("auto-increment column: %s",
				(autoinccolumn)?autoinccolumn:"(null");
			debugWrite("primary key column (from db): %s",
				(primarykeycolumn)?primarykeycolumn:"(null)");
		}
		debugEnd();
	}

	// if parseInsert didn't find a primary key
	// then try to get it from the configuration
	if (!primarykeycolumn) {
		primarykeycolumn=tablenode->
					getFirstTagChild("primarykey")->
					getAttributeValue("name");
		debugWrite("primary key column (from config): %s",
				(primarykeycolumn)?primarykeycolumn:"(null)");
	}

	// debug
	if (getDebug()) {
		debugStart("values");
		if (vals) {
			for (listnode<char *> *node=vals->getFirst();
						node; node=node->getNext()) {
				debugWrite("%s",node->getValue());
			}
		}
		debugEnd();
		debugStart("where-clause columns");
		for (domnode *node=tablenode->getFirstTagChild("column");
				!node->isNullNode();
				node=node->getNextTagSibling("column")) {
			debugWrite("%s",node->getAttributeValue("name"));
		}
		debugEnd();
	}


	// Create a separate cursor to run the update rather than just using
	// the cursor that ran the original insert.  This preserves the insert
	// cursor in case the client wants to use it to reexecute the insert.
	sqlrservercursor	*ucur=cont->newCursor();
	if (!ucur) {
		cont->setError(icur,"upsert failed - "
					"failed to create update cursor",
					SQLR_ERROR_TRIGGER,true);
		debugWrite("upsert failed - failed to create update cursor");
		debugEnd();
		delete[] table;
		delete cols;
		delete vals;
		return false;
	}

	// open the update cursor
	if (!cont->open(ucur)) {
		cont->setError(icur,"upsert failed - "
					"failed to open update cursor",
					SQLR_ERROR_TRIGGER,true);
		debugWrite("upsert failed - failed to open update cursor");
		debugEnd();
		delete[] table;
		delete cols;
		delete vals;
		return false;
	}

	// copy input binds from icur to ucur, convert the insert
	// to an update, then prepare and execute the update query
	// (each of these sets the error message internally if it fails)
	stringbuffer		update;
	bool	success=copyInputBinds(ucur,icur,cols,vals,tablenode) &&
				convertInsertToUpdate(ucur,table,
						cols,vals,
						autoinccolumn,primarykeycolumn,
						tablenode,&update) &&
				cont->prepareQuery(ucur,update.getString(),
							update.getSize()) &&
				cont->executeQuery(ucur);
	if (success) {

		// icur currenty contains the error that
		// triggered the upsert, clear that error
		cont->clearError();
		cont->clearError(icur);

		// copy affected rows back to icur
		cont->setAffectedRows(icur,cont->getAffectedRows(ucur));

	} else {
		// copy the error from the cursor used to run the
		// update to the cursor used to run the original insert
        	const char      *errorstring;
        	uint32_t        errorsize;
        	int64_t         errnum;
        	bool            liveconnection;
        	cont->getError(ucur,&errorstring,
					&errorsize,
                                       	&errnum,
					&liveconnection);
		cont->setError(icur,errorstring,errorsize,
					errnum,liveconnection);
		debugWrite("error: %d - %.*s",errnum,errorsize,errorstring);
	}

	debugEnd();

	// clean up
	if (ucur) {
		cont->closeResultSet(ucur);
		cont->close(ucur);
		cont->deleteCursor(ucur);
	}
	delete[] table;
	delete cols;
	delete vals;
	return success;
}

bool sqlrtrigger_upsert::errorEncountered(sqlrservercursor *icur) {

	// the error buffer may not be terminated, but contains() below
	// needs a terminated string, so make a copy of it here
	stringbuffer	err;
	err.append(cont->getErrorBuffer(icur),cont->getErrorSize(icur));

	// FIXME: this is somewhat inefficient, copy xml to a list of
	// conditions like I'm doing in the replay module

	// look through the errors and see if we find
	// one that matches the icur's error
	for (domnode *node=errors->getFirstTagChild("error");
				!node->isNullNode();
				node=node->getNextTagSibling("error")) {
		const char	*string=node->getAttributeValue("string");
		const char	*number=node->getAttributeValue("number");
		if ((string && charstring::contains(err.getString(),string)) ||
			(number && cont->getErrorNumber(icur)==
					charstring::convertToInteger(number))) {
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

bool sqlrtrigger_upsert::copyInputBinds(sqlrservercursor *ucur,
					sqlrservercursor *icur,
					linkedlist<char *> *cols,
					linkedlist<char *> *vals,
					domnode *tablenode) {

	settowhere.clear();

	// bail if there are no input binds
	uint16_t	ibcount=cont->getInputBindCount(icur);
	if (!ibcount) {
		return true;
	}

	debugStart("bind-to-col map");

	// build a bind -> col map
	dictionary<char *, const char *>	bindtocol;
	bindtocol.setManageArrayKeys(true);
	uint16_t		bindnum=1;
	listnode<char *>	*cnode=cols->getFirst();
	listnode<char *>	*vnode=vals->getFirst();
	while (cnode && vnode) {

		// get the column/value pair
		const char	*col=cnode->getValue();
		const char	*val=vnode->getValue();

		// if val is a bind variable then map
		// it to the corresponding column
		if (isBind(val)) {

			if (cont->getBindFormat()[0]=='?') {

				// we only support bind by position...

				// val wil just be a ?, append
				// the bind number to it
				char	*bindname;
				charstring::printf(&bindname,"?%hd",bindnum);
				bindtocol.setValue(bindname,col);
				bindnum++;
				debugWrite("%s -> %s",bindname,col);

			} else {

				// we support bind by name/number
				bindtocol.setValue(
					charstring::duplicate(val),col);
				debugWrite("%s -> %s",val,col);
			}
		}

		// next...
		cnode=cnode->getNext();
		vnode=vnode->getNext();
	}

	// make 2 copies of icur's input binds in ucur:
	// * one of each bind to use in the set clause
	// * one of each bind to use in the where clause

	// run through the binds, counting the ones
	// that we'll need to make copies of
	sqlrserverbindvar	*ivars=cont->getInputBinds(icur);
	sqlrserverbindvar	*uvars=cont->getInputBinds(ucur);
	uint16_t		ubcount=0;
	for (uint16_t i=0; i<ibcount; i++) {

		// for the set clause, copy all bind vars
		ubcount++;

		// for the where clause, only copy the bind vars that
		// correspond to columns that will be used in the where clause
		if (tablenode->getFirstTagChild("column","name",
				bindtocol.getValue(ivars[i].variable))->
				isNullNode()) {
			continue;
		}
		ubcount++;
	}

	// make sure we can allocate as many binds as we need
	if (ubcount>cont->getConfig()->getMaxBindCount()) {
		cont->setError(ucur,"upsert failed - update would "
					"exceed maximum bind count",
					SQLR_ERROR_TRIGGER,true);
		return false;
	}

	debugEnd();

	if (ibcount) {
		debugStart("binds:");
	}

	// copy the input binds, making one copy for the set clause and
	// another copy for the where clause
	memorypool	*upool=cont->getBindPool(ucur);
	uint16_t	ui=ibcount;
	for (uint16_t i=0; i<ibcount; i++) {

		// for the set clause, copy all bind vars
		copyInputBind(upool,false,&(uvars[i]),&(ivars[i]),i);

		// for the where clause, only copy the bind vars that
		// correspond to columns that will be used in the where clause
		if (tablenode->getFirstTagChild("column","name",
				bindtocol.getValue(ivars[i].variable))->
				isNullNode()) {
			continue;
		}
		copyInputBind(upool,true,&(uvars[ui]),&(ivars[i]),ui+1);
		ui++;
	}

	// set the input bind count
	cont->setInputBindCount(ucur,ubcount);

	if (ibcount) {
		debugEnd();
	}

	return true;
}

void sqlrtrigger_upsert::copyInputBind(memorypool *pool, bool where,
						sqlrserverbindvar *ubind,
						sqlrserverbindvar *ibind,
						uint16_t bindnumber) {

	// byte-copy everything
	bytestring::copy(ubind,ibind,sizeof(sqlrserverbindvar));

	// The shallow-copy above will aim the variable, value.stringval,
	// and value.dateval.buffer pointers to the strings stored in the
	// main cursor's memorypool.  There's no need to make a copy of
	// those strings, as they will persist for as long as these binds do.

	// So, for the copy of the bind that we'll use in the set clause,
	// we can bail here.
	if (!where) {
		debugWrite("%s=%s",
			ubind->variable,
			((ubind->type==SQLRSERVERBINDVARTYPE_STRING)?
					ubind->value.stringval:"..."));
		return;
	}

	// We do need to rename the variable for the copy of the bind that
	// we'll use in the where clause though....

	if (charstring::contains(cont->getBindFormat(),'*')) {

		// if we support named binds, then prepend "where_"
		// to the variable name
		ubind->variablesize+=6;
		ubind->variable=(char *)pool->allocate(ubind->variablesize+1);
		charstring::printf(ubind->variable,
					ubind->variablesize+1,
					"%c%s%s",
					ibind->variable[0],
					"where_",
					ibind->variable+1);

		// map the set ->where variable name for
		// easier lookup when building the update query
		settowhere.setValue(ibind->variable,ubind->variable);

	} else {

		// if we only support numeric binds or bind-by-position,
		// then use the bind number that we were passed in
		ubind->variablesize=1+charstring::getIntegerLength(bindnumber);
		ubind->variable=(char *)pool->allocate(ubind->variablesize+1);
		charstring::printf(ubind->variable,
					ubind->variablesize+1,
					"%c%hd",
					ibind->variable[0],
					bindnumber);

		// unless we only support bind-by-position...
		if (cont->getBindFormat()[0]!='?') {

			// map the set -> where bind variable name for
			// easier lookup when building the update query
			settowhere.setValue(ibind->variable,ubind->variable);
		}
	}

	debugWrite("%s=%s",
		ubind->variable,
		((ubind->type==SQLRSERVERBINDVARTYPE_STRING)?
				ubind->value.stringval:"..."));
	debugWrite("%s -> %s",ibind->variable,ubind->variable);
}

bool sqlrtrigger_upsert::convertInsertToUpdate(
					sqlrservercursor *ucur,
					const char *table,
					linkedlist<char *> *cols,
					linkedlist<char *> *vals,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					domnode *tablenode,
					stringbuffer *query) {

	// begin building the update query
	query->append("update ")->append(table)->append(" set ");

	debugStart("col-to-val map");

	// build the set clause and map column names to values
	dictionary<const char *, const char *>	coltoval;
	bool			first=true;
	listnode<char *>	*cnode=cols->getFirst();
	listnode<char *>	*vnode=vals->getFirst();
	while (cnode && vnode) {

		// get the column/value pair
		const char	*col=cnode->getValue();
		const char	*val=vnode->getValue();

		// don't attempt to set auto-increment or primary key columns
		if (!charstring::compare(col,autoinccolumn) ||
			!charstring::compare(col,primarykeycolumn)) {
 			cnode=cnode->getNext();
 			vnode=vnode->getNext();
			continue;
		}

		// append the column/value pair to the set clause
		if (first) {
			first=false;
		} else {
			query->append(',');
		}
		query->append(col)->append('=')->append(val);

		// map column -> value for use in the where clause later
		coltoval.setValue(col,val);

		debugWrite("%s -> %s",col,val);

		// next...
 		cnode=cnode->getNext();
 		vnode=vnode->getNext();
	}

	// begin building the where clause
	query->append(" where ");

	// build the where clause
	bool	retval=true;
	first=true;
	for (domnode *node=tablenode->getFirstTagChild("column");
				!node->isNullNode();
				node=node->getNextTagSibling("column")) {

		// get the column/value pair
		const char	*col=node->getAttributeValue("name");
		const char	*val;
		if (!coltoval.getValue(col,&val)) {
			// bail if we didn't find a value for this column
			stringbuffer	err;
			err.append("upsert failed - in conversion of "
					"insert to update, no value was found "
					"in the original insert for column: ")->
					append(col);
			cont->setError(ucur,err.getString(),
						err.getSize(),
						SQLR_ERROR_TRIGGER,true);
			retval=false;
			break;
		}

		// append them to the where clause
		if (!first) {
			query->append(" and ");
		}
		query->append(col)->append('=');

		// If "val" is a bind variable (and not just a ?) then append
		// the corresponding bind variable that we created earlier in
		// copyInputBinds for use in the where clause.
		// If "val" is not a bind variable (or it is, but it's just a ?)
		// then append "val" literally.
		if (isBind(val) && val[0]!='?') {
			query->append(settowhere.getValue(val));
		} else {
			query->append(val);
		}
		first=false;
	}

	debugWrite("update query:\n");
	debugWrite(query->getString());
	debugEnd();
	
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

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_upsert(sqlrservercontroller *cont,
						domnode *parameters) {

		return new sqlrtrigger_upsert(cont,parameters);
	}
}
