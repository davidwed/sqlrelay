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
		querydetails();
		~querydetails();

		char		*query;
		uint32_t	querysize;
		linkedlist<sqlrserverbindvar *>	*inbindvars;
		linkedlist<sqlrserverbindvar *>	*outbindvars;
		linkedlist<sqlrserverbindvar *>	*inoutbindvars;
};

querydetails::querydetails() {
	query=NULL;
	querysize=0;
	inbindvars=NULL;
	outbindvars=NULL;
	inoutbindvars=NULL;
}

querydetails::~querydetails() {
	delete inbindvars;
	delete outbindvars;
	delete inoutbindvars;
}

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
						domnode *parameters);
		bool	runAfter(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);

		void	endTransaction(bool commit);

	private:
		void	logQuery(sqlrservercursor *sqlrcur);
		bool	replay(sqlrservercursor *sqlrcur,
					condition *cond);
		condition	*replayCondition(sqlrservercursor *sqlrcur);
		void	writeReplayConditionToLogFile(condition *cond,
						sqlrservercursor *sqlrcur);
		void	writeToLogFile(const char *logfile,
					const char *str, size_t size);


		void	disableUntilEndOfTx(const char *query, 
						int32_t querysize,
						sqlrquerytype_t querytype);
		void	copyQuery(querydetails *qd,
					const char *query,
					uint32_t querysize);
		void	rewriteQuery(querydetails *qd,
					const char *query,
					uint32_t querysize,
					linkedlist<char *> *columns,
					const char *autoinccolumn,
					uint64_t liid,
					bool columnsincludeautoinccolumn,
					const char *values);
		void	appendValues(const char *values,
						const char *queryend,
						linkedlist<char *> *columns,
						uint64_t liid,
						const char *autoinccolumn);

		bool		includeselects;
		uint32_t	maxretries;

		stringbuffer	err;

		stringbuffer	newquery;
		stringbuffer	value;

		linkedlist<querydetails *>	log;
		linkedlist<condition *>		conditions;
		memorypool			logpool;

		stringbuffer	logstr;

		bool	logqueries;

		bool	wasintx;

		bool	disabled;
};

sqlrtrigger_replay::sqlrtrigger_replay(sqlrservercontroller *cont,
					domnode *parameters) :
					sqlrtrigger(cont,parameters) {

	log.setManageValues(true);
	conditions.setManageValues(true);

	// get whether to include selects
	includeselects=charstring::isYes(
			parameters->getAttributeValue("includeselects"));

	// get the max retries
	maxretries=charstring::convertToInteger(
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
			c->errorcode=charstring::convertToInteger(err);
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
					getAttributeValue("file");
		// formerly, the log file was part of the query tag
		if (!c->logfile) {
			c->logfile=cond->getFirstTagChild("log")->
					getFirstTagChild("query")->
					getAttributeValue("file");
		}
		c->query=cond->getFirstTagChild("log")->
					getFirstTagChild("query")->
					getFirstChild("text")->getValue();

		conditions.append(c);
	}

	logqueries=true;

	wasintx=false;

	disabled=false;
}

bool sqlrtrigger_replay::runAfter(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// bail if the query was suppressed
	if (cont->getQuerySuppressed(sqlrcur)) {
		return true;
	}

	// bail if logging/replay was disabled due to a query we can't handle
	if (disabled) {
		return true;
	}

	// log the query
	logQuery(sqlrcur);

	// bail if we didn't encounter a replay condition
	condition	*cond=replayCondition(sqlrcur);
	if (!cond) {
		return true;
	}

	// replay the log if the query failed because of a replay condition
	return (cond)?replay(sqlrcur,cond):true;
}

void sqlrtrigger_replay::logQuery(sqlrservercursor *sqlrcur) {

	// bail if we're not supposed to be logging
	if (!logqueries) {
		return;
	}

	// If we're not in a transaction, we only need
	// to log the current query.  Clear the log.
	if (!cont->getInTransaction()) {
		logpool.clear();
		log.clear();
	}

	// If we weren't in a transaction, but are now,
	// then we also need to clear the log.
	if (cont->getInTransaction() && !wasintx) {
		logpool.clear();
		log.clear();
		wasintx=true;
	}

	// get the last insert id, we'll need it later, and it might
	// be reset by parseInsert(), so we need to get it here
	uint64_t	liid=0;
	bool		gotliid=cont->getLastInsertId(&liid);

	// get query type
	const char		*query=sqlrcur->getQueryBuffer();
	uint32_t		querysize=sqlrcur->getQuerySize();
	sqlrquerytype_t		querytype=SQLRQUERYTYPE_ETC;
	linkedlist<char *>	*columns=NULL;
	linkedlist<char *>	*allcolumns=NULL;
	const char 		*autoinccolumn=NULL;
	bool			columnsincludeautoinccolumn=false;
	const char		*rawvalues=NULL;
	cont->parseInsert(query,querysize,
				&querytype,
				NULL,
				&columns,
				&allcolumns,
				&autoinccolumn,
				&columnsincludeautoinccolumn,
				NULL,NULL,NULL,
				&rawvalues);

	// bail if the query was a select, and we're ignoring selects
	if (!includeselects && querytype==SQLRQUERYTYPE_SELECT) {
		debugWrite("ignoring query:");
		debugWrite("%.*s",sqlrcur->getQuerySize(),
					sqlrcur->getQueryBuffer());
		delete columns;
		return;
	}

	// We can't select-into during replay.
	if (querytype==SQLRQUERYTYPE_SELECTINTO) {
		disableUntilEndOfTx(query,querysize,querytype);
		delete columns;
		return;
	}

	// log the query...
	querydetails	*qd=new querydetails;
	if (querytype==SQLRQUERYTYPE_INSERT ||
		 querytype==SQLRQUERYTYPE_MULTIINSERT) {

// FIXME: there's a case we're not handling...  if the query contains a null
// for the auto-increment column, then we need to replace it with the
// last-insert-id

		if (!gotliid || !autoinccolumn || columnsincludeautoinccolumn) {

			// If there was no last-insert-id or auto-increment
			// column, or if there was an auto-increment column,
			// but it was included in the insert, then we don't
			// actually have to rewrite anything.  Just do a normal
			// copy.
			copyQuery(qd,query,querysize);

		} else if (querytype==SQLRQUERYTYPE_INSERT) {

			rewriteQuery(qd,query,querysize,
					columns,autoinccolumn,liid,
					columnsincludeautoinccolumn,rawvalues);

		} else {

			// The query was apparently a multi-insert, with
			// an autoincrement column, which generated an id.
			// There's no way (currently) to handle these.
			disableUntilEndOfTx(query,querysize,querytype);
			delete columns;
			return;
		}

	} else if (querytype==SQLRQUERYTYPE_INSERTSELECT) {

		// There's no way (currently) to handle these.
		disableUntilEndOfTx(query,querysize,querytype);
		delete columns;
		return;

	} else {
		copyQuery(qd,query,querysize);
	}


	// copy in input binds
	uint16_t	incount=sqlrcur->getInputBindCount();
	if (incount) {
		sqlrserverbindvar	*invars=
					sqlrcur->getInputBinds();
		qd->inbindvars=new linkedlist<sqlrserverbindvar *>();
		for (uint16_t i=0; i<incount; i++) {
			sqlrserverbindvar	*bv=new sqlrserverbindvar;
			cont->copyBind(&(invars[i]),bv,&logpool);
			qd->inbindvars->append(bv);
		}
	}
	
	// copy in output binds
	uint16_t	outcount=sqlrcur->getOutputBindCount();
	if (outcount) {
		sqlrserverbindvar	*outvars=
					sqlrcur->getOutputBinds();
		qd->outbindvars=new linkedlist<sqlrserverbindvar *>();
		for (uint16_t i=0; i<outcount; i++) {
			sqlrserverbindvar	*bv=new sqlrserverbindvar;
			cont->copyBind(&(outvars[i]),bv,&logpool);
			qd->outbindvars->append(bv);
		}
	}

	// copy in input-output binds
	uint16_t	inoutcount=sqlrcur->getInputOutputBindCount();
	if (inoutcount) {
		sqlrserverbindvar	*inoutvars=
					sqlrcur->getInputOutputBinds();
		qd->inoutbindvars=new linkedlist<sqlrserverbindvar *>();
		for (uint16_t i=0; i<inoutcount; i++) {
			sqlrserverbindvar	*bv=new sqlrserverbindvar;
			cont->copyBind(&(inoutvars[i]),bv,&logpool);
			qd->inoutbindvars->append(bv);
		}
	}

	// log copied query and binds
	log.append(qd);

#if 0
	debugWrite("-----------------------");
	for (listnode<querydetails *> *node=log.getFirst();
					node; node=node->getNext()) {
		debugWrite(node->getValue()->query);
	}
	debugWrite("-----------------------\n");
#endif

	delete columns;
}

void sqlrtrigger_replay::disableUntilEndOfTx(const char *query, 
						int32_t querysize,
						sqlrquerytype_t querytype) {

	// If we weren't in a transaction, then just don't log the
	// query.  If we weren't in a transaction, then clear the log
	// and disable replay altogether until end-of-transaction.
	if (cont->getInTransaction()) {

		logpool.clear();
		log.clear();
		disabled=true;

		// write message to all log files
		for (listnode<condition *> *node=conditions.getFirst();
						node; node=node->getNext()) {

			// clear log buffer
			logstr.clear();

			// delimiter
			logstr.append("========================================"
				"=======================================\n");

			// timestamp
			datetime	dt;
			dt.initFromSystemDateTime();
			logstr.append(dt.getString())->append("\n\n");

			if (querytype==SQLRQUERYTYPE_INSERTSELECT) {
				logstr.append("insert-select");
			} else if (querytype==SQLRQUERYTYPE_SELECTINTO){
				logstr.append("select-into");
			} else {
				logstr.append("multi-insert");
			}
			logstr.append(" query encountered, "
					"disabling replay until "
					"end-of-transaction:\n");
			logstr.append(query,querysize);
			logstr.append("\n\n");

			writeToLogFile(node->getValue()->logfile,
							logstr.getString(),
							logstr.getSize());
		}
	}
}

void sqlrtrigger_replay::copyQuery(querydetails *qd,
					const char *query,
					uint32_t querysize) {

	// copy query verbatim
	qd->querysize=querysize;
	qd->query=(char *)logpool.allocate(querysize+1);
	bytestring::copy(qd->query,query,querysize);
	// (make sure to null terminate)
	qd->query[querysize]='\0';
}

void sqlrtrigger_replay::rewriteQuery(querydetails *qd,
					const char *query,
					uint32_t querysize,
					linkedlist<char *> *columns,
					const char *autoinccolumn,
					uint64_t liid,
					bool columnsincludeautoinccolumn,
					const char *rawvalues) {
	newquery.clear();

	// did the query contain column names?

	// skip to the start of the query
	const char	*start=cont->skipWhitespaceAndComments(query);

	// skip to table name
	const char	*table=start+12;

	// skip to either "(" before columns or "values"
	// FIXME: the table name could be quoted and contain a space
	const char	*colsstart=charstring::findFirst(table,' ')+1;

	// append up to the columns
	newquery.append(start,colsstart-start);

	// append columns
	newquery.append('(');
	if (!columnsincludeautoinccolumn) {
		newquery.append(autoinccolumn)->append(',');
	}
	bool	first=true;
	for (listnode<char *> *node=columns->getFirst();
					node; node=node->getNext()) {
		if (first) {
			first=false;
		} else {
			newquery.append(',');
		}
		newquery.append(node->getValue());
	}

	// append values
	newquery.append(") values (");
	if (!columnsincludeautoinccolumn) {
		newquery.append(liid)->append(',');
		newquery.append(rawvalues,querysize-(rawvalues-query));
	} else {
		appendValues(rawvalues,query+querysize,
					columns,liid,autoinccolumn);
	}

	// copy out the rewritten query
	copyQuery(qd,newquery.getString(),newquery.getSize());
}

void sqlrtrigger_replay::appendValues(const char *values,
						const char *queryend,
						linkedlist<char *> *columns,
						uint64_t liid,
						const char *autoinccolumn) {
	value.clear();

	listnode<char *>	*col=columns->getFirst();
	const char		*c=values;
	uint32_t		parens=0;
	for (;;) {

		// handle quotes
		if (*c=='\'') {
			const char	*after=
				charstring::findEndOfQuotedString(
						c,queryend-c,'\'',true,true);
			value.append(c,after-c-1);
			c=after;
		}

		// handle parens
		if (*c=='(') {

			parens++;
			value.append(*c);

		} else if (*c==')') {

			if (parens) {

				parens--;
				value.append(*c);

			} else {

				// if the value was a null and this is
				// the autoincrement column, then
				// append the last-insert-id,
				// otherwise just append the value
				if (!charstring::compare(col->getValue(),
							autoinccolumn) &&
					!charstring::compare(
							value.getString(),
							"null")) {
					newquery.append(liid);
				} else {
					newquery.append(
						value.getString());
				}

				// append the )
				newquery.append(')');

				return;
			}

		} else

		// handle commas between values
		if (*c==',') {

			// if the value was a null and this is the
			// autoincrement column, then append the
			// last-insert-id, otherwise just append the value
			if (!charstring::compare(col->getValue(),
						autoinccolumn) &&
				!charstring::compare(
						value.getString(),
						"null")) {
				newquery.append(liid);
			} else {
				newquery.append(value.getString());
			}

			// append the comma
			newquery.append(',');

			col=col->getNext();
			value.clear();

		} else

		// handle all other characters
		{
			value.append(*c);
		}

		// keep going
		c++;
	}
}

bool sqlrtrigger_replay::replay(sqlrservercursor *sqlrcur, condition *cond) {

	// clear log buffer
	logstr.clear();

	// delimiter
	logstr.append("----------------------------------------"
			"---------------------------------------\n");
	logstr.append("log replay...\n\n");

	// don't log any queries that we run during the replay
	logqueries=false;

	// get the bind pool
	memorypool	*pool=cont->getBindPool(sqlrcur);

	debugStart("replay");
	debugWrite("triggering query:");
	debugWrite("%.*s",sqlrcur->getQuerySize(),sqlrcur->getQueryBuffer());

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
				(cond->replaytx)?log.getFirst():log.getLast();

	// replay...
	while (current) {

		// get the query details
		querydetails	*qd=current->getValue();
		
		// prepare the query
		debugStart("prepare query");
		debugWrite("query:");
		debugWrite("%.*s",qd->querysize,qd->query);

		if (!cont->prepareQuery(sqlrcur,qd->query,qd->querysize)) {

			debugWrite("prepare error: %.*s",
					sqlrcur->getErrorSize(),
					sqlrcur->getErrorBuffer());

			logstr.append("prepare error:\n");
			logstr.append(qd->query,qd->querysize);
			logstr.append("\n");
			logstr.append(sqlrcur->getErrorBuffer(),
					sqlrcur->getErrorSize());
			logstr.append("\n\n");

			retval=false;
			break;
		}
		debugEnd();

		// initialize bind counts
		sqlrcur->setInputBindCount(0);
		sqlrcur->setOutputBindCount(0);
		sqlrcur->setInputOutputBindCount(0);

		// copy out input binds
		if (qd->inbindvars) {
			uint16_t	incount=qd->inbindvars->getCount();
			sqlrcur->setInputBindCount(incount);
			sqlrserverbindvar	*invars=
						sqlrcur->getInputBinds();
			if (incount) {
				debugStart("input binds");
			}
			listnode<sqlrserverbindvar *>	*inbindnode=
						qd->inbindvars->getFirst();
			for (uint16_t i=0; i<incount; i++) {
				sqlrserverbindvar	*bv=
						inbindnode->getValue();
				debugWrite("%.*s",
					bv->variablesize,bv->variable);
				cont->copyBind(&(invars[i]),bv,pool);
				inbindnode=inbindnode->getNext();
			}
			if (incount) {
				debugEnd();
			}
		}

		// copy out output binds
		if (qd->outbindvars) {
			uint16_t	outcount=qd->outbindvars->getCount();
			sqlrcur->setOutputBindCount(outcount);
			sqlrserverbindvar	*outvars=
						sqlrcur->getOutputBinds();
			if (outcount) {
				debugStart("output binds");
			}
			listnode<sqlrserverbindvar *>	*outbindnode=
						qd->outbindvars->getFirst();
			for (uint16_t i=0; i<outcount; i++) {
				sqlrserverbindvar	*bv=
						outbindnode->getValue();
				debugWrite("%.*s",
					bv->variablesize,bv->variable);
				cont->copyBind(&(outvars[i]),bv,pool);
				outbindnode=outbindnode->getNext();
			}
			if (outcount) {
				debugEnd();
			}
		}

		// copy out input-output binds
		if (qd->inoutbindvars) {
			uint16_t	inoutcount=
					qd->inoutbindvars->getCount();
			sqlrcur->setInputOutputBindCount(inoutcount);
			sqlrserverbindvar	*inoutvars=
						sqlrcur->getInputOutputBinds();
			if (inoutcount) {
				debugStart("input-output binds");
			}
			listnode<sqlrserverbindvar *>	*inoutbindnode=
						qd->inoutbindvars->getFirst();
			for (uint16_t i=0; i<inoutcount; i++) {
				sqlrserverbindvar	*bv=
						inoutbindnode->getValue();
				debugWrite("%.*s",
					bv->variablesize,bv->variable);
				cont->copyBind(&(inoutvars[i]),bv,pool);
				inoutbindnode=inoutbindnode->getNext();
			}
			if (inoutcount) {
				debugEnd();
			}
		}

		// execute the query
		debugStart("execute query");

		if (!cont->executeQuery(sqlrcur)) {

			// if this fails, then it's actually ok, the
			// query may have failed to execute in the
			// original tx too...
			debugWrite("execute error: %.*s",
					sqlrcur->getErrorSize(),
					sqlrcur->getErrorBuffer());

			logstr.append("execute error:\n");
			logstr.append(qd->query,qd->querysize);
			logstr.append("\n");
			logstr.append(sqlrcur->getErrorBuffer(),
					sqlrcur->getErrorSize());
			logstr.append("\n\n");
		}
		debugEnd();

		// if the execute failed because of a replay condition...
		if (replayCondition(sqlrcur)) {

			// bump retry count
			retry++;
		
			// bail if we've tried too many times already
			if (maxretries && retry>maxretries) {
				logstr.append(
					"deadlocks occurred during replay,");
				logstr.append(" max retries (");
				logstr.append(maxretries);
				logstr.append(") reached at query:\n");
				logstr.append(qd->query,qd->querysize);
				logstr.append("\n");
				logstr.append(sqlrcur->getErrorBuffer(),
						sqlrcur->getErrorSize());
				logstr.append("\n\n");
				retval=false;
				break;
			}

			if (cond->replaytx) {

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
				debugWrite("delay %d sec, %d usec...",sec,usec);
				snooze::microsnooze(sec,usec);
			}

		} else {

			// advance to the next query in the log
			current=current->getNext();
		}
	}

	debugEnd();

	if (!retval) {
		// roll back and clear the log on error
		cont->rollback();
		logpool.clear();
		log.clear();
	} else {
		logstr.append("success!\n\n");
	}

	// start logging queries again
	logqueries=true;

	writeToLogFile(cond->logfile,logstr.getString(),logstr.getSize());

	return retval;
}

condition *sqlrtrigger_replay::replayCondition(sqlrservercursor *sqlrcur) {

	// bail if we didn't get an error
	if (!cont->getErrorSize(sqlrcur) && !cont->getErrorNumber(sqlrcur)) {
		return NULL;
	}

	// the error buffer may not be terminated, but contains() below
	// needs a terminated string, so make a copy of it here
	err.clear();
	err.append(cont->getErrorBuffer(sqlrcur),cont->getErrorSize(sqlrcur));

	// did we get a replay condition?
	for (listnode<condition *> *node=conditions.getFirst();
						node; node=node->getNext()) {

		condition	*cond=node->getValue();

		if (cond->cond==CONDITION_ERROR) {
			if (charstring::contains(err.getString(),cond->error)) {
				writeReplayConditionToLogFile(cond,sqlrcur);
				return cond;
			}
		} else if (cond->cond==CONDITION_ERRORCODE) {
			if (cont->getErrorNumber(sqlrcur)==cond->errorcode) {
				writeReplayConditionToLogFile(cond,sqlrcur);
				return cond;
			}
		}
	}
	return NULL;
}

void sqlrtrigger_replay::writeReplayConditionToLogFile(condition *cond,
						sqlrservercursor *sqlrcur) {

	// bail if we don't have a logfile to log to
	if (!cond->logfile) {
		return;
	}

	// clear log buffer
	logstr.clear();

	// delimiter
	logstr.append("========================================"
			"=======================================\n");

	// timestamp
	datetime	dt;
	dt.initFromSystemDateTime();
	logstr.append(dt.getString())->append("\n\n");

	// replay condition
	logstr.append("replay condition detected...\n\n");
	logstr.append("triggering query:\n");
	logstr.append(sqlrcur->getQueryBuffer(),sqlrcur->getQuerySize());
	logstr.append("\n\n");
	if (cond->cond==CONDITION_ERROR) {
		logstr.append("error string: ");
		logstr.append(sqlrcur->getErrorBuffer(),
					sqlrcur->getErrorSize());
		logstr.append("\n");
		logstr.append("matching error pattern: ");
		logstr.append(cond->error);
		logstr.append("\n");
	} else if (cond->cond==CONDITION_ERRORCODE) {
		logstr.append("error code: ");
		logstr.append(cond->errorcode);
		logstr.append("\n");
	}
	logstr.append("requires full replay: ");
	logstr.append((cond->replaytx)?"true":"false");
	logstr.append("\n\n");

	// run log query and write results to log file...
	if (cond->query) {

		// don't log this query
		logqueries=false;

		// delimiter
		logstr.append("----------------------------------------"
				"---------------------------------------\n");

		// run query
		sqlrservercursor        *logcur=cont->newCursor();
		bool	success=cont->open(logcur);
		if (!success) {
			logstr.append("failed to open log query cursor\n\n");
		}
		if (success) {
			success=cont->prepareQuery(logcur,cond->query,
					charstring::getLength(cond->query));
			if (!success) {
        			const char      *errorstring;
        			uint32_t        errorsize;
        			int64_t         errnum;
        			bool            liveconnection;
        			cont->getError(logcur,&errorstring,
							&errorsize,
                                        		&errnum,
							&liveconnection);
				logstr.append("failed to prepare log query:\n");
				logstr.append(cond->query);
				logstr.append("\n");
				logstr.append(errorstring,errorsize);
				logstr.append("\n\n");
			}
		}
		if (success) {
			success=cont->executeQuery(logcur);
			if (!success) {
        			const char      *errorstring;
        			uint32_t        errorsize;
        			int64_t         errnum;
        			bool            liveconnection;
        			cont->getError(logcur,&errorstring,
							&errorsize,
                                        		&errnum,
							&liveconnection);
				logstr.append("failed to execute log query:\n");
				logstr.append(cond->query);
				logstr.append("\n");
				logstr.append(errorstring,errorsize);
				logstr.append("\n\n");
			}
		}
		if (success) {
			success=cont->colCount(logcur);
			if (!success) {
				logstr.append(
					"log query produced no columns\n\n");
			}
		}

		if (success) {

			bool	first=true;
			bool    error;
			while (cont->fetchRow(logcur,&error)) {

				if (first) {
					first=false;
				}

				// get fields
				for (uint32_t i=0;
					i<cont->colCount(logcur); i++) {

					const char	*field;
					uint64_t	fieldsize;
					bool		lob;
					bool		null;
					cont->getField(logcur,i,&field,
						&fieldsize,&lob,&null);

					logstr.append(
						cont->getColumnName(logcur,i));
					logstr.append(" : ");
					if (fieldsize>
						(uint64_t)(80-
						cont->getColumnNameSize(
								logcur,i)-4)) {
						logstr.append('\n');
					}
					logstr.append(field,fieldsize);
					logstr.append('\n');
				}
				logstr.append('\n');

				// FIXME: kludgy
				cont->nextRow(logcur);
			}

			if (first) {
				logstr.append("log query produced no rows\n\n");
			}
		}
		cont->closeResultSet(logcur);
		cont->close(logcur);
		cont->deleteCursor(logcur);

		// start logging queries again
		logqueries=true;
	}

	// write transaction log to log file
	logstr.append("----------------------------------------"
			"---------------------------------------\n");
	logstr.append("transaction log:\n\n");
	for (listnode<querydetails *> *node=log.getFirst();
					node; node=node->getNext()) {
		logstr.append(node->getValue()->query)->append("\n\n");
	}

	writeToLogFile(cond->logfile,logstr.getString(),logstr.getSize());
}

void sqlrtrigger_replay::writeToLogFile(const char *logfile,
					const char *str, size_t size) {

	// bail if there's nowhere to write to or nothing to write
	if (!logfile || !str || !size) {
		return;
	}

	// open log file
	file	lf;
	if (!lf.open(logfile,O_WRONLY|O_APPEND|O_CREAT,
				permissions::parsePermString("rw-r--r--"))) {
		if (getDebug()) {
			char	*err=error::getErrorString();
			debugWrite("failed to open %s",logfile);
			debugWrite(logfile,err);
			delete[] err;
			return;
		}
	}

	// write the log message all-at-once
	lf.write(str,size);

	debugWrite("%.*s",size,str);
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
						domnode *parameters) {

		return new sqlrtrigger_replay(cont,parameters);
	}
}
