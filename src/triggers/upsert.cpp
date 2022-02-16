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
		bool	copyInputBinds(sqlrservercursor *dest,
					sqlrservercursor *source);
		void	copyInputBind(memorypool *pool,
					bool where,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source,
					uint16_t bindindex);
		bool	convertInsertToUpdate(
					sqlrservercursor *ucur,
					const char *table,
					const char * const *cols,
					uint64_t colcount,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					const char *values,
					domnode *tablenode,
					stringbuffer *query);
		bool	isBind(const char *var);
		void	deleteCols(char **cols, uint64_t colcount);

		sqlrservercontroller	*cont;

		bool	debug;

		domnode	*errors;
		domnode	*tables;

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
						sqlrservercursor *icur,
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
	const char		*query=icur->getQueryBuffer();
	uint32_t		querylen=icur->getQueryLength();
	sqlrquerytype_t		querytype=icur->queryType(query,querylen);

	if (debug) {
		stdoutput.printf("upsert {\n");
		stdoutput.printf("	triggering query:\n%.*s\n",
							querylen,query);
	}

	// bail if the query wasn't an insert, or if the insert didn't
	// encounter an error that should trigger an update
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		if (debug) {
			stdoutput.printf("	query was not an insert\n}\n");
		}
		return *success;
	}

	// bail if insert didn't encounter an
	// error that should trigger an update
	if (!errorEncountered(icur)) {
		if (debug) {
			stdoutput.printf("	no matching error "
					"found for:\n%d: %s}\n",
					icur->getErrorNumber(),
					icur->getErrorBuffer());
		}
		return *success;
	}

	// parse the query
	// NOTE: parseInsert will populate querytype with a more specific value
	char		*table=NULL;
	char		**cols=NULL;
	uint64_t	colcount=0;
	const char	*autoinccolumn=NULL;
	const char	*primarykeycolumn=NULL;
	const char	*values=NULL;
	cont->parseInsert(query,querylen,&querytype,
				&table,&cols,&colcount,NULL,
				&autoinccolumn,NULL,
				&primarykeycolumn,NULL,
				&values);

	if (debug) {
		stdoutput.printf("	table: %s\n",table);
		stdoutput.printf("	columns:\n");
		for (uint64_t i=0; i<colcount; i++) {
			stdoutput.printf("		%s\n",cols[i]);
		}
		stdoutput.printf("	auto-increment column: %s\n",
							autoinccolumn);
		stdoutput.printf("	primary key column (from db): %s\n",
							primarykeycolumn);
		stdoutput.printf("	values: %s\n",values);
	}

	// bail if the query wasn't a simple insert
	if (querytype!=SQLRQUERYTYPE_INSERT) {
		if (debug) {
			stdoutput.printf("	not a simple insert\n}\n");
		}
		deleteCols(cols,colcount);
		delete[] table;
		return *success;
	}

	// bail if the table isn't one that we want to update
	domnode	*tablenode=tableEncountered(table);
	if (!tablenode) {
		if (debug) {
			stdoutput.printf("	table not "
					"configured for upsert\n}\n");
		}
		deleteCols(cols,colcount);
		delete[] table;
		return *success;
	}

	// if parseInsert didn't find a primary key
	// then try to get it from the table node
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

	// If we made it here, then the original insert failed with some error
	// that triggered all of this to happen.  So *success=false.  Reset it
	// to true.  We'll set it false again if some step below fails.
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
	// (each of these sets the error message internally if they fail)
	stringbuffer		update;
	if (*success) {
		*success=copyInputBinds(ucur,icur) &&
				convertInsertToUpdate(ucur,table,cols,colcount,
						autoinccolumn,primarykeycolumn,
						values,tablenode,&update) &&
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
			cont->setError(icur,errorstring,errnum,liveconnection);
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
	deleteCols(cols,colcount);
	delete[] table;
	return *success;
}

bool sqlrtrigger_upsert::errorEncountered(sqlrservercursor *icur) {

	// look through the errors and see if we find
	// one that matches the icur's error
	// FIXME: maybe cache the number using
	// domnode::setData() for future lookups
	for (domnode *node=errors->getFirstTagChild("error");
				!node->isNullNode();
				node=node->getNextTagSibling("error")) {
		const char	*string=node->getAttributeValue("string");
		const char	*number=node->getAttributeValue("number");
		if ((string && charstring::contains(
					icur->getErrorBuffer(),string)) ||
			(number && icur->getErrorNumber()==
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
bool sqlrtrigger_upsert::copyInputBinds(sqlrservercursor *dest,
					sqlrservercursor *source) {

	wherebinds.clear();

	// make 2 copies of source's input binds in dest:
	// * one of each bind to use in the set clause
	// * one of each bind to use in the where clause

	// count source's binds and make sure we
	// can allocate twice as many in dest
	uint16_t	ibcount=cont->getInputBindCount(source);
	if (ibcount*2>cont->getConfig()->getMaxBindCount()) {
		cont->setError(dest,"upsert failed - update would "
					"exceed maximum bind count",
					SQLR_ERROR_TRIGGER,true);
		return false;
	}

	if (ibcount && debug) {
		stdoutput.printf("	binds:\n");
	}

	// copy the input binds, making one copy for the set clause and
	// another copy for the where clause
	memorypool		*destpool=cont->getBindPool(dest);
	sqlrserverbindvar	*sinvars=cont->getInputBinds(source);
	sqlrserverbindvar	*dinvars=cont->getInputBinds(dest);
	for (uint16_t i=0; i<ibcount; i++) {
		copyInputBind(destpool,
			false,&(dinvars[i]),&(sinvars[i]),i);
		copyInputBind(destpool,
			true,&(dinvars[ibcount+i]),&(sinvars[i]),ibcount+i);
	}

	// set the input bind count
	cont->setInputBindCount(dest,ibcount*2);

	return true;
}

void sqlrtrigger_upsert::copyInputBind(memorypool *pool, bool where,
						sqlrserverbindvar *dest,
						sqlrserverbindvar *source,
						uint16_t bindindex) {

	// byte-copy everything
	bytestring::copy(dest,source,sizeof(sqlrserverbindvar));

	// The shallow-copy above will aim the variable, value.stringval,
	// and value.dateval.buffer pointers to the strings stored in the
	// main cursor's memorypool.  There's no need to make a copy of
	// those strings, as they will persist for as long as these binds do.

	// So, for the copy of the bind that we'll use in the set clause,
	// we can bail here.
	if (!where) {
		if (debug) {
			stdoutput.printf("		%s=",dest->variable);
			if (dest->type==SQLRSERVERBINDVARTYPE_STRING) {
				stdoutput.printf("%s\n",dest->value.stringval);
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
		dest->variablesize+=6;
		dest->variable=(char *)pool->allocate(dest->variablesize+1);
		charstring::printf(dest->variable,
					dest->variablesize+1,
					"%c%s%s",
					source->variable[0],
					"where_",
					source->variable+1);

		// map the source -> dest variable name for
		// easier lookup when building the update query
		wherebinds.setValue(source->variable,dest->variable);

	} else {

		// if we only support numeric binds or bind-by-position,
		// then use the bind index that we were passed in
		// NOTE: bindindex is 0 based, but numeric bind names are
		// 1-based, so we'll add one to bindindex to get the numeric
		// bind name
		dest->variablesize=1+charstring::integerLength(bindindex+1);
		dest->variable=(char *)pool->allocate(dest->variablesize+1);
		charstring::printf(dest->variable,
					dest->variablesize+1,
					"%c%hd",
					source->variable[0],
					bindindex+1);

		// unless we only support bind-by-position...
		if (cont->bindFormat()[0]!='?') {

			// map the source -> dest variable name for
			// easier lookup when building the update query
			wherebinds.setValue(source->variable,dest->variable);
		}
	}

	if (debug) {
		stdoutput.printf("		%s=",dest->variable);
		if (dest->type==SQLRSERVERBINDVARTYPE_STRING) {
			stdoutput.printf("%s\n",dest->value.stringval);
		} else {
			stdoutput.printf("...\n");
		}
		stdoutput.printf("			%s -> %s\n",
					source->variable,dest->variable);
	}
}

bool sqlrtrigger_upsert::convertInsertToUpdate(
					sqlrservercursor *ucur,
					const char *table,
					const char * const *cols,
					uint64_t colcount,
					const char *autoinccolumn,
					const char *primarykeycolumn,
					const char *values,
					domnode *tablenode,
					stringbuffer *query) {

	// split values
	char		**vals;
	uint64_t	valscount;
	// FIXME: use a split that considers quoting and ignores the trailing )
	char	*tempvalues=charstring::duplicate(values);
	tempvalues[charstring::length(tempvalues)-1]='\0';
	charstring::split(tempvalues,",",false,&vals,&valscount);
	delete[] tempvalues;

	// begin building the update query
	query->append("update ")->append(table)->append(" set ");

	// build the set clause and map column names to values
	dictionary<const char *,const char *>	valuemap;
	bool	first=true;
	for (uint64_t i=0; i<colcount; i++) {

		// get the column/value pair
		const char	*col=cols[i];
		const char	*val=vals[i];

		// don't attempt to set auto-increment or primary key columns
		if (!charstring::compare(col,autoinccolumn) ||
			!charstring::compare(col,primarykeycolumn)) {
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
		valuemap.setValue(col,val);
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
		if (!valuemap.getValue(col,&val)) {
			// bail if we didn't find a value for this column
			stringbuffer	err;
			err.append("upsert failed - in conversion of "
					"insert to update, no value was found "
					"in the original insert for column: ")->
					append(col);
			cont->setError(ucur,err.getString(),
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
			query->append(wherebinds.getValue(val));
		} else {
			query->append(val);
		}
		first=false;
	}

	if (debug) {
		stdoutput.printf("	update query:\n%s\n",
						query->getString());
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
