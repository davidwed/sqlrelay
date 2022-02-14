// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>
#include <rudiments/linkedlist.h>
#include <rudiments/dictionary.h>
#include <rudiments/character.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>
#include <rudiments/error.h>
#include <rudiments/snooze.h>

class querydetails {
	public:
		char		*query;
		uint32_t	querylen;
		linkedlist<sqlrserverbindvar *>	inbindvars;
		linkedlist<sqlrserverbindvar *>	outbindvars;
		linkedlist<sqlrserverbindvar *>	inoutbindvars;
};

enum condition_t {
	CONDITION_ERROR=0,
	CONDITION_ERRORCODE
};

class condition {
	public:
		condition_t	cond;
		const char	*error;
		uint32_t	errorcode;
		bool		replaytx;

		// for now we only support logging the result of a query
		const char	*query;
		const char	*logfile;
};

class SQLRSERVER_DLLSPEC sqlrtrigger_replay : public sqlrtrigger {
	public:
		sqlrtrigger_replay(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success);

		void	endTransaction(bool commit);

	private:
		bool	logQuery(sqlrservercursor *sqlrcur);
		bool	replay(sqlrservercursor *sqlrcur,
					bool replaytx);
		bool	replayCondition(sqlrservercursor *sqlrcur,
					bool *replaytx,
					bool indent);
		void	logReplayCondition(condition *cond);


		void	disableUntilEndOfTx(const char *query, 
						int32_t querylen,
						sqlrquerytype_t querytype);
		void	copyQuery(querydetails *qd,
					const char *query,
					uint32_t querylen);
		void	rewriteQuery(querydetails *qd,
					const char *query,
					uint32_t querylen,
					char **cols,
					uint64_t colcount,
					const char *autoinccolumn,
					uint64_t liid,
					bool columnsincludeautoinccolumn);
		void	deleteCols(char **cols, uint64_t colcount);
		void	appendValues(stringbuffer *newquery,
						const char *values,
						char **cols,
						uint64_t liid,
						const char *autoinccolumn);

		void	copyBind(memorypool *pool,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source);

		sqlrservercontroller	*cont;

		bool		debug;
		bool		includeselects;
		uint32_t	maxretries;

		linkedlist<querydetails *>	log;
		linkedlist<condition *>		conditions;
		memorypool			logpool;

		bool	logqueries;

		bool	wasintx;

		bool	disabled;
};

sqlrtrigger_replay::sqlrtrigger_replay(sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	this->cont=cont;

	debug=cont->getConfig()->getDebugTriggers();

	log.setManageValues(true);
	conditions.setManageValues(true);

	// get whether to include selects
	includeselects=charstring::isYes(
			parameters->getAttributeValue("includeselects"));

	// get the max retries
	maxretries=charstring::toInteger(
				parameters->getAttributeValue("maxretries"));

	// get the replay conditions...
	for (domnode *cond=parameters->getFirstTagChild("condition");
				!cond->isNullNode();
				cond=cond->getNextTagSibling("condition")) {

		condition	*c=new condition;

		// for now we only support <condition error="..."/>
		const char	*err=cond->getAttributeValue("error");
		if (charstring::isNumber(err)) {
			c->cond=CONDITION_ERRORCODE;
			c->errorcode=charstring::toInteger(err);
		} else {
			c->cond=CONDITION_ERROR;
			c->error=err;
		}

		// get the scope (query or tx)
		c->replaytx=!charstring::compareIgnoringCase(
					cond->getAttributeValue("scope"),
					"transaction");

		// In the future, we might allow multiple queries/commands to
		// be run when this condition occurs, and log the output.  But
		// for now we only support logging the result of a single query.
		// Get the query and file to log to, if provided...
		c->logfile=cond->getFirstTagChild("log")->
					getFirstTagChild("query")->
					getAttributeValue("file");
		c->query=cond->getFirstTagChild("log")->
					getFirstTagChild("query")->
					getFirstChild("text")->getValue();

		conditions.append(c);
	}

	logqueries=true;

	wasintx=false;

	disabled=false;
}

bool sqlrtrigger_replay::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success) {

	// only run this trigger after running a query
	if (before) {
		return *success;
	}

	// bail if logging/replay was disabled due to a query we can't handle
	if (disabled) {
		return *success;
	}

	// bail if we couldn't log the query for some reason
	if (!logQuery(sqlrcur)) {
		*success=false;
		return false;
	}

	// bail if the query failed (*success==false)
	// but we didn't encounter a replay condition
	bool	replaycondition=false;
	bool	replaytx=false;
	if (!(*success)) {
		replaycondition=replayCondition(sqlrcur,&replaytx,false);
		if (!replaycondition) {
			*success=false;
			return false;
		}
	}

	// replay the log if the query failed because of a replay condition
	if (replaycondition) {
		*success=replay(sqlrcur,replaytx);
	}
	return *success;
}

bool sqlrtrigger_replay::logQuery(sqlrservercursor *sqlrcur) {

	// bail if we're not supposed to be logging
	if (!logqueries) {
		return true;
	}

	// If we're not in a transaction, we only need
	// to log the current query.  Clear the log.
	if (!cont->inTransaction()) {
		logpool.clear();
		log.clear();
	}

	// If we weren't in a transaction, but are now,
	// then we also need to clear the log.
	if (cont->inTransaction() && !wasintx) {
		logpool.clear();
		log.clear();
		wasintx=true;
	}

	// get query type
	const char		*query=sqlrcur->getQueryBuffer();
	uint32_t		querylen=sqlrcur->getQueryLength();
	sqlrquerytype_t		querytype=SQLRQUERYTYPE_ETC;
	char			**cols=NULL;
	uint64_t		colcount=0;
	linkedlist<char *>	*allcolumns=NULL;
	const char 		*autoinccolumn=NULL;
	bool			columnsincludeautoinccolumn=false;
	uint64_t		liid=0;
	cont->parseInsert(query,querylen,
			&querytype,
			&cols,&colcount,
			&allcolumns,
			&autoinccolumn,
			&columnsincludeautoinccolumn,
			&liid);

	// bail if the query was a select, and we're ignoring selects
	if (!includeselects && querytype==SQLRQUERYTYPE_SELECT) {
		if (debug) {
			stdoutput.printf("ignoring query:\n%.*s\n}\n",
						sqlrcur->getQueryLength(),
						sqlrcur->getQueryBuffer());
		}
		deleteCols(cols,colcount);
		return true;
	}

	// We can't select-into during replay.
	if (querytype==SQLRQUERYTYPE_SELECTINTO) {
		disableUntilEndOfTx(query,querylen,querytype);
		deleteCols(cols,colcount);
		return true;
	}

	// log the query...
	querydetails	*qd=new querydetails;
	if (querytype==SQLRQUERYTYPE_INSERT ||
		 querytype==SQLRQUERYTYPE_MULTIINSERT) {

// FIXME: there's a case we're not handling...  if the query contains a null
// for the auto-increment column, then we need to replace it with the
// last-insert-id

		if (!liid || !autoinccolumn || columnsincludeautoinccolumn) {

			// If there was no last-insert-id or auto-increment
			// column, or if there was an auto-increment column,
			// but it was included in the insert, then we don't
			// actually have to rewrite anything.  Just do a normal
			// copy.
			copyQuery(qd,query,querylen);

		} else if (querytype==SQLRQUERYTYPE_INSERT) {

			rewriteQuery(qd,query,querylen,
					cols,colcount,autoinccolumn,liid,
					columnsincludeautoinccolumn);

		} else {

			// The query was apparently a multi-insert, with
			// an autoincrement column, which generated an id.
			// There's no way (currently) to handle these.
			disableUntilEndOfTx(query,querylen,querytype);
			deleteCols(cols,colcount);
			return true;
		}

	} else if (querytype==SQLRQUERYTYPE_INSERTSELECT) {

		// There's no way (currently) to handle these.
		disableUntilEndOfTx(query,querylen,querytype);
		deleteCols(cols,colcount);
		return true;

	} else {
		copyQuery(qd,query,querylen);
	}

	// copy in input binds
	uint16_t		incount=sqlrcur->getInputBindCount();
	sqlrserverbindvar	*invars=sqlrcur->getInputBinds();
	for (uint16_t i=0; i<incount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(&logpool,bv,&(invars[i]));
		qd->inbindvars.append(bv);
	}
	
	// copy in output binds
	uint16_t		outcount=sqlrcur->getOutputBindCount();
	sqlrserverbindvar	*outvars=sqlrcur->getOutputBinds();
	for (uint16_t i=0; i<outcount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(&logpool,bv,&(outvars[i]));
		qd->outbindvars.append(bv);
	}

	// copy in input-output binds
	uint16_t		inoutcount=sqlrcur->getInputOutputBindCount();
	sqlrserverbindvar	*inoutvars=sqlrcur->getInputOutputBinds();
	for (uint16_t i=0; i<inoutcount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(&logpool,bv,&(inoutvars[i]));
		qd->inoutbindvars.append(bv);
	}

	// log copied query and binds
	log.append(qd);

	/*if (debug) {
		stdoutput.printf("-----------------------\n");
		for (listnode<querydetails *> *node=log.getFirst();
						node; node=node->getNext()) {
			stdoutput.printf("%s\n",node->getValue()->query);
		}
	}*/
	deleteCols(cols,colcount);
	return true;
}

void sqlrtrigger_replay::disableUntilEndOfTx(const char *query, 
						int32_t querylen,
						sqlrquerytype_t querytype) {

	// If we weren't in a transaction, then just don't log the
	// query.  If we weren't in a transaction, then clear the log
	// and disable replay altogether until end-of-transaction.
	if (cont->inTransaction()) {
		logpool.clear();
		log.clear();
		disabled=true;
		if (debug) {
			stdoutput.printf("%s query encountered, "
				"disabling replay until "
				"end-of-transaction:\n%.*s\n}\n",
				((querytype==SQLRQUERYTYPE_INSERTSELECT)?
							"insert-select":
				((querytype==SQLRQUERYTYPE_SELECTINTO)?
							"select-into":
				"multi-insert")),
				querylen,query);
		}
	}
}

void sqlrtrigger_replay::copyQuery(querydetails *qd,
					const char *query,
					uint32_t querylen) {

	// copy query verbatim
	qd->querylen=querylen;
	qd->query=(char *)logpool.allocate(querylen+1);
	bytestring::copy(qd->query,query,querylen);
	// (make sure to null terminate)
	qd->query[querylen]='\0';
}

void sqlrtrigger_replay::rewriteQuery(querydetails *qd,
					const char *query,
					uint32_t querylen,
					char **cols,
					uint64_t colcount,
					const char *autoinccolumn,
					uint64_t liid,
					bool columnsincludeautoinccolumn) {
	stringbuffer	newquery;

	// did the query contain column names?

	// skip to the start of the query
	const char	*start=cont->skipWhitespaceAndComments(query);

	// skip to table name
	const char	*table=start+12;

	// skip to either "(" before columns or "values"
	// FIXME: the table name could be quoted and contain a space
	const char	*colsstart=charstring::findFirst(table,' ')+1;

	// skip to first value (after ") values (")
	// FIXME: kind-of a kludge...
	// sometimes queries are written:
	//	insert into blah values(...);
	// with no space after "values", and the normalize translation
	// doesn't fix this (though it ought to)
	const char	*values=charstring::findFirst(colsstart,"values(");
	if (values) {
		values+=7;
	} else {
		values=charstring::findFirst(colsstart,"values (");
		if (values) {
			values+=8;
		}
	}

	// append up to the columns
	newquery.append(start,colsstart-start);

	// append columns
	newquery.append('(');
	if (!columnsincludeautoinccolumn) {
		newquery.append(autoinccolumn)->append(',');
	}
	for (uint64_t i=0; i<colcount; i++) {
		if (i) {
			newquery.append(',');
		}
		newquery.append(cols[i]);
	}

	// append values
	newquery.append(") values (");
	if (!columnsincludeautoinccolumn) {
		newquery.append(liid)->append(',')->append(values);
	} else {
		appendValues(&newquery,values,cols,liid,autoinccolumn);
	}

	// copy out the rewritten query
	copyQuery(qd,newquery.getString(),newquery.getStringLength());
}

void sqlrtrigger_replay::deleteCols(char **cols, uint64_t colcount) {

	// clean up
	for (uint64_t i=0; i<colcount; i++) {
		delete[] cols[i];
	}
	delete[] cols;
}

void sqlrtrigger_replay::appendValues(stringbuffer *newquery,
						const char *values,
						char **cols,
						uint64_t liid,
						const char *autoinccolumn) {

	stringbuffer	value;
	uint64_t	valueindex=0;
	const char	*c=values;
	char		prevc='\0';
	bool		inquotes=false;
	uint32_t	parens=0;
	for (;;) {

		// end-of-values condition
		if (!inquotes && !parens && *c==')') {

			// if the value was a null and this is
			// the autoincrement column, then
			// append the last-insert-id,
			// otherwise just append the value
			if (!charstring::compare(cols[valueindex],
							autoinccolumn) &&
				!charstring::compare(value.getString(),
								"null")) {
				newquery->append(liid);
			} else {
				newquery->append(
					value.getString());
			}

			// append the )
			newquery->append(')');

			return;
		}

		// handle quotes...
		if (!inquotes && *c=='\'') {
			inquotes=true;
		} else if (inquotes && *c=='\'' && prevc!='\\') {
			inquotes=false;
		}

		if (!inquotes) {

			// handle parentheses...
			if (*c=='(') {
				parens++;
				value.append(*c);
			} else if (parens && *c==')') {
				parens--;
				value.append(*c);
			} else {
				if (*c==',') {

					// if the value was a null and this is
					// the autoincrement column, then
					// append the last-insert-id,
					// otherwise just append the value
					if (!charstring::compare(
							cols[valueindex],
							autoinccolumn) &&
						!charstring::compare(
							value.getString(),
							"null")) {
						newquery->append(liid);
					} else {
						newquery->append(
							value.getString());
					}

					// append the comma
					newquery->append(',');

					valueindex++;
					value.clear();
				} else {
					value.append(*c);
				}
			}
		} else {
			value.append(*c);
		}

		// keep going
		prevc=*c;
		c++;
	}
}

void sqlrtrigger_replay::copyBind(memorypool *pool,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source) {

	// byte-copy everything
	bytestring::copy(dest,source,sizeof(sqlrserverbindvar));

	// (re)copy variable name
	dest->variablesize=source->variablesize;
	dest->variable=(char *)pool->allocate(dest->variablesize+1);
	charstring::copy(dest->variable,source->variable);
	
	// (re)copy strings
	if (source->type==SQLRSERVERBINDVARTYPE_STRING) {
		dest->value.stringval=
			(char *)pool->allocate(source->valuesize+1);
		charstring::copy(dest->value.stringval,
					source->value.stringval);
	} else if (source->type==SQLRSERVERBINDVARTYPE_DATE) {
		dest->value.dateval.tz=
			(char *)pool->allocate(
				charstring::length(source->value.dateval.tz)+1);
		charstring::copy(dest->value.dateval.tz,
					source->value.dateval.tz);
		dest->value.dateval.buffer=
			(char *)pool->allocate(
				source->value.dateval.buffersize);
		charstring::copy(dest->value.dateval.buffer,
					source->value.dateval.buffer,
					source->value.dateval.buffersize);
	}
}

bool sqlrtrigger_replay::replay(sqlrservercursor *sqlrcur,
					bool replaytx) {

	// don't log any queries that we run during the replay
	logqueries=false;

	// get the bind pool
	memorypool	*pool=cont->getBindPool(sqlrcur);

	if (debug) {
		stdoutput.printf("replay {\n");
		stdoutput.printf("	triggering query:\n%.*s\n",
					sqlrcur->getQueryLength(),
					sqlrcur->getQueryBuffer());
	}

	// clear the triggering query's error
	cont->clearError();
	cont->clearError(sqlrcur);

	// init return value
	bool		retval=true;

	// init retry count
	uint32_t	retry=0;

	// init delay parameters
	uint32_t	sec=0;
	uint32_t	usec=0;

	// get the start query...
	// If we're replaying the entire tx then start at the beginning of the
	// log.  If we're just replaying the last query, then start at the end
	// of the log.
	listnode<querydetails *> *current=
				(replaytx)?log.getFirst():log.getLast();

	// replay...
	while (current) {

		// get the query details
		querydetails	*qd=current->getValue();
		
		// prepare the query
		if (debug) {
			stdoutput.printf("	prepare query {\n");
			stdoutput.printf("		query:\n%.*s\n",
						qd->querylen,qd->query);
		}
		if (!cont->prepareQuery(sqlrcur,qd->query,qd->querylen)) {
			if (debug) {
				stdoutput.printf(
					"		"
					"prepare error: %.*s\n",
					sqlrcur->getErrorLength(),
					sqlrcur->getErrorBuffer());
				stdoutput.printf("	}\n");
			}
			retval=false;
			break;
		}
		if (debug) {
			stdoutput.printf("	}\n");
		}

		// copy out input binds
		uint16_t	incount=qd->inbindvars.getLength();
		sqlrcur->setInputBindCount(incount);
		sqlrserverbindvar	*invars=
					sqlrcur->getInputBinds();
		if (debug && incount) {
			stdoutput.printf("	input binds {\n");
		}
		listnode<sqlrserverbindvar *>	*inbindnode=
						qd->inbindvars.getFirst();
		for (uint16_t i=0; i<incount; i++) {
			sqlrserverbindvar	*bv=
					inbindnode->getValue();
			if (debug) {
				stdoutput.printf("		%.*s\n",
						bv->variablesize,
						bv->variable);
			}
			copyBind(pool,&(invars[i]),bv);
			inbindnode=inbindnode->getNext();
		}
		if (debug && incount) {
			stdoutput.printf("	}\n");
		}

		// copy out output binds
		uint16_t	outcount=qd->outbindvars.getLength();
		sqlrcur->setInputBindCount(outcount);
		sqlrserverbindvar	*outvars=
					sqlrcur->getOutputBinds();
		if (debug && outcount) {
			stdoutput.printf("	output binds {\n");
		}
		listnode<sqlrserverbindvar *>	*outbindnode=
					qd->outbindvars.getFirst();
		for (uint16_t i=0; i<outcount; i++) {
			sqlrserverbindvar	*bv=
					outbindnode->getValue();
			if (debug) {
				stdoutput.printf("		%.*s\n",
						bv->variablesize,
						bv->variable);
			}
			copyBind(pool,&(outvars[i]),bv);
			outbindnode=outbindnode->getNext();
		}
		if (debug && outcount) {
			stdoutput.printf("	}\n");
		}

		// copy out input-output binds
		uint16_t		inoutcount=
					qd->inoutbindvars.getLength();
		sqlrcur->setInputBindCount(inoutcount);
		sqlrserverbindvar	*inoutvars=
					sqlrcur->getInputOutputBinds();
		if (debug && inoutcount) {
			stdoutput.printf("	"
					"input-output binds {\n");
		}
		listnode<sqlrserverbindvar *>	*inoutbindnode=
					qd->inoutbindvars.getFirst();
		for (uint16_t i=0; i<inoutcount; i++) {
			sqlrserverbindvar	*bv=
					inoutbindnode->getValue();
			if (debug) {
				stdoutput.printf("		%.*s\n",
						bv->variablesize,
						bv->variable);
			}
			copyBind(pool,&(inoutvars[i]),bv);
			inoutbindnode=inoutbindnode->getNext();
		}
		if (debug && inoutcount) {
			stdoutput.printf("	}\n");
		}

		// execute the query
		if (debug) {
			stdoutput.printf("	execute query {\n");
		}
		if (!cont->executeQuery(sqlrcur)) {
			// if this fails, then it's actually ok, the
			// query may have failed to execute in the
			// original tx too...
			if (debug) {
				stdoutput.printf(
					"		"
					"execute error: %.*s\n",
					sqlrcur->getErrorLength(),
					sqlrcur->getErrorBuffer());
			}
		}
		if (debug) {
			stdoutput.printf("	}\n");
		}

		// if the execute failed because of a replay condition...
		if (replayCondition(sqlrcur,&replaytx,true)) {

			// bump retry count
			retry++;
		
			// bail if we've tried too many times already
			if (maxretries && retry>maxretries) {
				break;
			}

			if (replaytx) {

				// if the replay condition requires a full log
				// replay, then reset the current query to the
				// first in the log
				current=log.getFirst();

			} else {
				
				// if the replay condition requires the current
				// query to be replayed then just don't advance
				// to the next query
			}

			// delay before trying again...
			// delay a little longer before each retry,
			// up to 10 seconds
			// FIXME: this ought to be configurable
			if (retry==1) {
				usec=10000;
			} else {
				if (sec) {
					sec*=2;
					if (sec>=10) {
						sec=10;
					}
				} else {
					usec*=2;
					if (usec>=1000000) {
						usec=0;
						sec=1;
					}
				}
			}
			if (sec || usec) {
				if (debug) {
					stdoutput.printf("	delay "
								"%d sec, "
								"%d usec...\n",
								sec,usec);
				}
				snooze::microsnooze(sec,usec);
			}

		} else {

			// advance to the next query in the log
			current=current->getNext();
		}
	}

	if (debug) {
		stdoutput.printf("}\n");
	}

	if (!retval) {
		// roll back and clear the log on error
		cont->rollback();
		logpool.clear();
		log.clear();
	}

	// start logging queries again
	logqueries=true;

	return retval;
}

bool sqlrtrigger_replay::replayCondition(sqlrservercursor *sqlrcur,
						bool *replaytx,
						bool indent) {

	// did we get a replay condition?
	for (listnode<condition *> *node=conditions.getFirst();
						node; node=node->getNext()) {

		condition	*val=node->getValue();

		if (val->cond==CONDITION_ERROR) {

			// FIXME: error buffer might not be terminated
			if (charstring::contains(
				sqlrcur->getErrorBuffer(),val->error)) {
				*replaytx=node->getValue()->replaytx;
				if (debug) {
					stdoutput.printf(
						"%sreplay condition "
						"detected {\n"
						"%s	"
						"pattern: %s\n"
						"%s	"
						"error string: %.*s\n"
						"%s	"
						"requires full replay: %s\n"
						"%s}\n",
						(indent)?"	":"",
						(indent)?"	":"",
						val->error,
						(indent)?"	":"",
						sqlrcur->getErrorLength(),
						sqlrcur->getErrorBuffer(),
						(indent)?"	":"",
						(*replaytx)?"true":"false",
						(indent)?"	":"");
				}
				logReplayCondition(val);
				return true;
			}

		} else if (val->cond==CONDITION_ERRORCODE) {

			if (sqlrcur->getErrorNumber()==val->errorcode) {
				*replaytx=node->getValue()->replaytx;
				if (debug) {
					stdoutput.printf(
						"%sreplay condition "
						"detected {\n"
						"%s	"
						"error code: %d\n"
						"%s	"
						"requires full replay: %s\n"
						"%s}\n",
						(indent)?"	":"",
						(indent)?"	":"",
						val->errorcode,
						(indent)?"	":"",
						(*replaytx)?"true":"false",
						(indent)?"	":"");
				}
				logReplayCondition(val);
				return true;
			}
		}
	}
	return false;
}

void sqlrtrigger_replay::logReplayCondition(condition *cond) {

	// bail if we don't have a query to run or logfile to log to
	if (!cond->query || !cond->logfile) {
		return;
	}

	// delimiter and timestamp
	datetime	dt;
	dt.getSystemDateAndTime();
	stringbuffer	str;
	str.append("========================================"
			"=======================================\n");
	str.append(dt.getString())->append("\n\n");

	// don't log this query
	logqueries=false;

	// run query
	sqlrservercursor        *logcur=cont->newCursor();
	bool	success=cont->open(logcur);
	if (!success && debug) {
		stdoutput.printf("failed to open log cursor\n");
	}
	if (success) {
		success=cont->prepareQuery(logcur,cond->query,
					charstring::length(cond->query));
		if (!success && debug) {
        		const char      *errorstring;
        		uint32_t        errorlength;
        		int64_t         errnum;
        		bool            liveconnection;
        		cont->errorMessage(logcur,&errorstring,
							&errorlength,
                                        		&errnum,
							&liveconnection);
			stdoutput.printf("failed to prepare log query:\n"
						"%s\n%.*s\n",cond->query,
						errorlength,errorstring);
		}
	}
	if (success) {
		success=cont->executeQuery(logcur);
		if (!success && debug) {
        		const char      *errorstring;
        		uint32_t        errorlength;
        		int64_t         errnum;
        		bool            liveconnection;
        		cont->errorMessage(logcur,&errorstring,
							&errorlength,
                                        		&errnum,
							&liveconnection);
			stdoutput.printf("failed to execute log query:\n"
						"%s\n%.*s\n",cond->query,
						errorlength,errorstring);
		}
	}
	if (success) {
		success=cont->colCount(logcur);
		if (!success && debug) {
			stdoutput.printf("log query produced no columns\n");
		}
	}

	if (success) {

		bool	first=true;
		bool    error;
		while (cont->fetchRow(logcur,&error)) {

			if (first) {
				first=false;
			} else {
				str.append(
				"----------------------------------------"
				"---------------------------------------\n");
			}

			// get fields
			for (uint32_t i=0; i<cont->colCount(logcur); i++) {

				const char	*field;
				uint64_t	fieldlength;
				bool		blob;
				bool		null;
				cont->getField(logcur,i,&field,
						&fieldlength,&blob,&null);

				str.append(cont->getColumnName(logcur,i));
				str.append(" : ");
				if (fieldlength>
					(uint64_t)(80-
					cont->getColumnNameLength(logcur,i)-
					4)) {
					str.append('\n');
				}
				str.append(field,fieldlength);
				str.append('\n');
			}
			str.append('\n');

			// FIXME: kludgy
			cont->nextRow(logcur);
		}

		if (first && debug) {
			stdoutput.printf("log query produced no rows\n");
		}
	}
	cont->closeResultSet(logcur);
	cont->close(logcur);
	cont->deleteCursor(logcur);

	// start logging queries again
	logqueries=true;

	// open log file
	file	logfile;
	if (!logfile.open(cond->logfile,
				O_WRONLY|O_APPEND|O_CREAT,
				permissions::evalPermString("rw-r--r--"))) {
		if (debug) {
			char	*err=error::getErrorString();
			stdoutput.printf("failed to open %s\n%s\n",
							cond->logfile,err);
			delete[] err;
			return;
		}
	}

	// write the log message all-at-once
	logfile.write(str.getString(),str.getSize());
}

void sqlrtrigger_replay::endTransaction(bool commit) {

	// bail if we're currently replaying the log...
	if (!logqueries) {
		return;
	}

	logpool.clear();
	log.clear();

	wasintx=false;

	disabled=false;
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_replay(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_replay(cont,ts,parameters);
	}
}
