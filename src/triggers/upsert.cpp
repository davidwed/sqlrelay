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
						sqlrtriggers *ts,
						domnode *parameters);

		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *icur,
						bool before,
						bool *success);
	private:
		bool	errorEncountered(sqlrservercursor *icur);
		domnode	*tableEncountered(const char *table);
		void	splitValues(const char *values,
					char ***vals, uint64_t *valcount);
		bool	copyInputBinds(sqlrservercursor *ucur,
					sqlrservercursor *icur,
					linkedlist<char *> *cols,
					const char * const *vals,
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
					const char * const *vals,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					domnode *tablenode,
					stringbuffer *query);
		bool	isBind(const char *var);
		void	deleteArray(char **vals, uint64_t valcount);

		sqlrservercontroller	*cont;

		bool	debug;

		domnode	*errors;
		domnode	*tables;

		dictionary<const char *, const char *>	settowhere;
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
						sqlrservercursor *icur,
						bool before,
						bool *success) {

	// before the query has been run, don't do anything
	if (before) {
		return *success;
	}

	// after the query has been run...

	// get the query and query type
	// NOTE: for now queryType() groups simple insert, multi-insert,
	// insert/select and select-into into SQLRQUERYTYPE_INSERT
	const char		*query=cont->getQueryBuffer(icur);
	uint32_t		querylen=cont->getQueryLength(icur);
	sqlrquerytype_t		querytype=icur->queryType(query,querylen);
	if (debug) {
		stdoutput.printf("upsert {\n");
		stdoutput.printf("	triggering query:\n%.*s\n",
							querylen,query);
	}

	// bail if the query wasn't an insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		if (debug) {
			stdoutput.printf("	query was not an insert\n}\n");
		}
		return *success;
	}

	// bail if the query didn't throw an error that we care about
	if (!errorEncountered(icur)) {
		if (debug) {
			stdoutput.printf("	no matching error "
					"found for:\n%d: %.*s}\n",
					cont->getErrorNumber(icur),
					cont->getErrorLength(icur),
					cont->getErrorBuffer(icur));
		}
		return *success;
	}

	// parse the query
	// NOTE: parseInsert will populate querytype with a more specific value
	char			*table=NULL;
	linkedlist<char *>	*cols=NULL;
	// FIXME: cols needs to be deleted eventually
	const char		*autoinccolumn=NULL;
	const char		*primarykeycolumn=NULL;
	const char		*values=NULL;
	cont->parseInsert(query,querylen,&querytype,
				&table,&cols,NULL,
				&autoinccolumn,NULL,
				&primarykeycolumn,NULL,
				&values);

	// debug
	if (debug) {
		stdoutput.printf("	table: %s\n",table);
		stdoutput.printf("	columns:\n");
		for (listnode<char *> *node=cols->getFirst();
						node; node=node->getNext()) {
			stdoutput.printf("		%s\n",node->getValue());
		}
		stdoutput.printf("	auto-increment column: %s\n",
							autoinccolumn);
		stdoutput.printf("	primary key column (from db): %s\n",
							primarykeycolumn);
	}

	// bail if the query wasn't a simple insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		if (debug) {
			stdoutput.printf("	not a simple insert\n}\n");
		}
		delete[] table;
		delete cols;
		return *success;
	}

	// bail if the table isn't one that we care about
	domnode	*tablenode=tableEncountered(table);
	if (!tablenode) {
		if (debug) {
			stdoutput.printf("	table not "
					"configured for upsert\n}\n");
		}
		delete[] table;
		delete cols;
		return *success;
	}

	// if parseInsert didn't find a primary key
	// then try to get it from the configuration
	if (!primarykeycolumn) {
		primarykeycolumn=tablenode->
					getFirstTagChild("primarykey")->
					getAttributeValue("name");
		if (debug) {
			stdoutput.printf("	primary key column "
						"(from config): %s\n",
						primarykeycolumn);
		}
	}

	// split values
	char		**vals;
	uint64_t	valcount;
	splitValues(values,&vals,&valcount);

	// debug
	if (debug) {
		stdoutput.printf("	values:\n");
		for (uint64_t i=0; i<valcount; i++) {
			stdoutput.printf("		%s\n",vals[i]);
		}
		stdoutput.printf("	where-clause columns:\n");
		for (domnode *node=tablenode->getFirstTagChild("column");
				!node->isNullNode();
				node=node->getNextTagSibling("column")) {
			stdoutput.printf("		%s\n",
					node->getAttributeValue("name"));
		}
	}

	// If we made it here, then the original insert failed with some error
	// that triggered all of this to happen.  So *success ought to be false.
	// Reset it to true.  We'll set it false again if some step below fails.
	*success=true;

	// Create an separate cursor to run the update rather than just using
	// the cursor that ran the original insert.  This preserves the insert
	// cursor in case the client wants to use it to reexecute the insert.
	sqlrservercursor	*ucur=cont->newCursor();
	if (!ucur) {
		*success=false;
		cont->setError(icur,"upsert failed - "
					"failed to create update cursor",
					SQLR_ERROR_TRIGGER,true);
	}

	// open the update cursor
	if (*success) {
		*success=cont->open(ucur);
		if (!*success) {
			cont->setError(icur,"upsert failed - "
						"failed to open update cursor",
						SQLR_ERROR_TRIGGER,true);
		}
	}

	// copy input binds from icur to ucur, convert the insert
	// to an update, then prepare and execute the update query
	// (each of these sets the error message internally if it fails)
	stringbuffer		update;
	if (*success) {
		*success=copyInputBinds(ucur,icur,cols,vals,tablenode) &&
				convertInsertToUpdate(ucur,table,
						cols,vals,
						autoinccolumn,primarykeycolumn,
						tablenode,&update) &&
				cont->prepareQuery(ucur,update.getString(),
						update.getStringLength()) &&
				cont->executeQuery(ucur);
		if (!*success) {
			// copy the error from the cursor used to run the
			// update to the cursor used to run the original insert
        		const char      *errorstring;
        		uint32_t        errorlength;
        		int64_t         errnum;
        		bool            liveconnection;
        		cont->errorMessage(ucur,&errorstring,
							&errorlength,
                                        		&errnum,
							&liveconnection);
			cont->setError(icur,errorstring,errorlength,
						errnum,liveconnection);
		}
	}

	// FIXME: Ideally we'd copy the affected rows from ucur to icur.
	// icur's affected rows will be 0 here because the insert failed.
	// Also, it's not impossible that ucur updated more than 1 row.  But,
	// the controller doesn't keep a copy of the affected rows, it just
	// returns them directly from the cursor, so, currently, there's no
	// way to set the affected rows.

	if (*success) {
		// icur currenty contains the error that
		// triggered the upsert, clear that error
		cont->clearError();
		cont->clearError(icur);
	}

	if (debug) {
		if (!*success) {
        		const char      *errorstring;
        		uint32_t        errorlength;
        		int64_t         errnum;
        		bool            liveconnection;
        		cont->errorMessage(icur,&errorstring,
							&errorlength,
                                        		&errnum,
							&liveconnection);
			stdoutput.printf("error: %d - %.*s\n",
					errnum,errorlength,errorstring);
		}
		stdoutput.printf("}\n");
	}

	// clean up
	if (ucur) {
		cont->closeResultSet(ucur);
		cont->close(ucur);
		cont->deleteCursor(ucur);
	}
	deleteArray(vals,valcount);
	delete[] table;
	delete cols;
	return *success;
}

bool sqlrtrigger_upsert::errorEncountered(sqlrservercursor *icur) {

	// the error buffer may not be terminated, but contains() below
	// needs a terminated string, so make a copy of it here
	stringbuffer	err;
	err.append(cont->getErrorBuffer(icur),cont->getErrorLength(icur));

	// look through the errors and see if we find
	// one that matches the icur's error
	for (domnode *node=errors->getFirstTagChild("error");
				!node->isNullNode();
				node=node->getNextTagSibling("error")) {
		const char	*string=node->getAttributeValue("string");
		const char	*number=node->getAttributeValue("number");
		if ((string && charstring::contains(err.getString(),string)) ||
			(number && cont->getErrorNumber(icur)==
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

void sqlrtrigger_upsert::splitValues(const char *values,
					char ***vals, uint64_t *valcount) {
	// FIXME: use a split that considers quoting and ignores the trailing )
	char	*tempvalues=charstring::duplicate(values);
	tempvalues[charstring::length(tempvalues)-1]='\0';
	charstring::split(tempvalues,",",false,vals,valcount);
	delete[] tempvalues;
}

bool sqlrtrigger_upsert::copyInputBinds(sqlrservercursor *ucur,
					sqlrservercursor *icur,
					linkedlist<char *> *cols,
					const char * const *vals,
					domnode *tablenode) {

	settowhere.clear();

	// bail if there are no input binds
	uint16_t	ibcount=cont->getInputBindCount(icur);
	if (!ibcount) {
		return true;
	}

	// build a bind -> col map
	dictionary<char *, const char *>	bindtocol;
	bindtocol.setManageArrayKeys(true);
	uint16_t	bindnum=1;
	if (debug) {
		stdoutput.printf("	bind-to-col map:\n");
	}

	uint64_t 	i=0;
	for (listnode<char *> *node=cols->getFirst();
				node; node=node->getNext()) {
		const char	*col=node->getValue();
		const char	*val=vals[i];
		if (isBind(val)) {
			if (cont->bindFormat()[0]=='?') {
				// If we only support bind-by-position then
				// val wil just be a ?.  In that case, append
				// the bind number to it.
				char	*bindname;
				charstring::printf(&bindname,"?%hd",bindnum);
				bindtocol.setValue(bindname,col);
				bindnum++;
				if (debug) {
					stdoutput.printf("		"
								"%s -> %s\n",
								bindname,col);
				}
			} else {
				bindtocol.setValue(
					charstring::duplicate(val),col);
				if (debug) {
					stdoutput.printf("		"
								"%s -> %s\n",
								val,col);
				}
			}
		}
		i++;
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

	if (ibcount && debug) {
		stdoutput.printf("	binds:\n");
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
		if (debug) {
			stdoutput.printf("		%s=",ubind->variable);
			if (ubind->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.printf("%s\n",ubind->value.stringval);
			} else {
				stdoutput.printf("...\n");
			}
		}
		return;
	}

	// We do need to rename the variable for the copy of the bind that
	// we'll use in the where clause though....

	if (charstring::contains(cont->bindFormat(),'*')) {

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
		ubind->variablesize=1+charstring::integerLength(bindnumber);
		ubind->variable=(char *)pool->allocate(ubind->variablesize+1);
		charstring::printf(ubind->variable,
					ubind->variablesize+1,
					"%c%hd",
					ibind->variable[0],
					bindnumber);

		// unless we only support bind-by-position...
		if (cont->bindFormat()[0]!='?') {

			// map the set -> where bind variable name for
			// easier lookup when building the update query
			settowhere.setValue(ibind->variable,ubind->variable);
		}
	}

	if (debug) {
		stdoutput.printf("		%s=",ubind->variable);
		if (ubind->type==SQLRSERVERBINDVARTYPE_STRING) {
			stdoutput.printf("%s\n",ubind->value.stringval);
		} else {
			stdoutput.printf("...\n");
		}
		stdoutput.printf("			%s -> %s\n",
					ibind->variable,ubind->variable);
	}
}

bool sqlrtrigger_upsert::convertInsertToUpdate(
					sqlrservercursor *ucur,
					const char *table,
					linkedlist<char *> *cols,
					const char * const *vals,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					domnode *tablenode,
					stringbuffer *query) {

	// begin building the update query
	query->append("update ")->append(table)->append(" set ");

	// build the set clause and map column names to values
	dictionary<const char *, const char *>	coltoval;
	bool	first=true;
	if (debug) {
		stdoutput.printf("	col-to-val map:\n");
	}
	uint64_t	i=0;
	for (listnode<char *> *node=cols->getFirst();
				node; node=node->getNext()) {

		// get the column/value pair
		const char	*col=node->getValue();
		const char	*val=vals[i];

		// don't attempt to set auto-increment or primary key columns
		if (!charstring::compare(col,autoinccolumn) ||
			!charstring::compare(col,primarykeycolumn)) {
			i++;
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

		if (debug) {
			stdoutput.printf("		%s -> %s\n",col,val);
		}
		i++;
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
						err.getStringLength(),
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

	if (debug) {
		stdoutput.printf("	update query:\n%s\n",
						query->getString());
	}
	
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


void sqlrtrigger_upsert::deleteArray(char **vals, uint64_t valcount) {
	for (uint64_t i=0; i<valcount; i++) {
		delete[] vals[i];
	}
	delete[] vals;
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_upsert(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_upsert(cont,ts,parameters);
	}
}
