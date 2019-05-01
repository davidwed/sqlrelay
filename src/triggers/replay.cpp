// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>
#include <rudiments/character.h>
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
};

class SQLRSERVER_DLLSPEC sqlrtrigger_replay : public sqlrtrigger {
	public:
		sqlrtrigger_replay(sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters);
		~sqlrtrigger_replay();
		bool	run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success);

		void	endTransaction(bool commit);

	private:
		bool	logQuery(sqlrservercursor *sqlrcur);
		bool	replayLog(sqlrservercursor *sqlrcur);
		bool	replayCondition(sqlrservercursor *sqlrcur);


		void	copyQuery(sqlrservercursor *sqlrcur,
						querydetails *qd);
		void	copyQuery(querydetails *qd,
					const char *query,
					uint32_t querylen);
		void	getQueryType(const char *query,
					uint32_t querylen,
					bool *isinsert,
					bool *isinsertselect,
					bool *isselectinto,
					bool *ismultiinsert);
		void	getColumns(const char *query,
					uint32_t querylen,
					linkedlist<char *> *columns,
					const char **autoinccolumn);
		void	rewriteQuery(querydetails *qd,
					const char *query,
					uint32_t querylen,
					uint64_t liid,
					linkedlist<char *> *columns,
					const char *autoinccolumn);
		uint64_t	countValues(const char *values);
		void		appendValues(stringbuffer *newquery,
						const char *values,
						char **cols,
						uint64_t liid,
						const char *autoinccolumn);

		void	copyBind(memorypool *pool,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source);

		sqlrservercontroller	*cont;

		bool		debug;

		bool		replaytx;

		uint32_t	maxretries;

		linkedlist<querydetails *>	log;
		linkedlist<condition *>		conditions;
		memorypool			logpool;

		bool	inreplay;
};

sqlrtrigger_replay::sqlrtrigger_replay(sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	this->cont=cont;

	debug=cont->getConfig()->getDebugTriggers();

	// get the scope (query or tx)
	replaytx=!charstring::compareIgnoringCase(
				parameters->getAttributeValue("scope"),
				"transaction");

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

		conditions.append(c);
	}

	inreplay=false;
}

sqlrtrigger_replay::~sqlrtrigger_replay() {
	conditions.clearAndDelete();
}

bool sqlrtrigger_replay::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success) {
	if (before) {
		return true;
	}

	*success=(logQuery(sqlrcur) && replayLog(sqlrcur) && *success);
	return *success;
}

bool sqlrtrigger_replay::logQuery(sqlrservercursor *sqlrcur) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return true;
	}

	if (replaytx) {

		// bail if we're not in a transaction
		if (!cont->inTransaction()) {
			return true;
		}

	} else {

		// clear the log and logpool
		logpool.clear();
		log.clearAndDelete();
	}

	// log the query...

	querydetails	*qd=new querydetails;

	// copy the query itself
	copyQuery(sqlrcur,qd);

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

	/*stdoutput.printf("-----------------------\n");
	for (linkedlistnode<querydetails *> *node=log.getFirst();
					node; node=node->getNext()) {
		stdoutput.printf("%s\n",node->getValue()->query);
	}*/
	return true;
}

void sqlrtrigger_replay::copyQuery(sqlrservercursor *sqlrcur,
						querydetails *qd) {

	const char	*query=sqlrcur->getQueryBuffer();
	uint32_t	querylen=sqlrcur->getQueryLength();

	// get query type
	bool	isinsert=false;
	bool	isinsertselect=false;
	bool	isselectinto=false;
	bool	ismultiinsert=false;
	getQueryType(query,querylen,
			&isinsert,&isinsertselect,
			&isselectinto,&ismultiinsert);

	// FIXME: suppprt insert select, select into, and multi-insert
	if (isinsert) {

		// get last insert id
		// (get it here because getColumns() below will reset it)
		uint64_t	liid=0;
		cont->getLastInsertId(&liid);

		// get columns
		linkedlist<char *>	columns;
		const char 		*autoinccolumn=NULL;
		getColumns(query,querylen,&columns,&autoinccolumn);

		if (!liid || !autoinccolumn) {

			// If there was no last-insert-id or auto-increment
			// column then we don't actually have to rewrite
			// anything.  Just do a normal copy.
			copyQuery(qd,query,querylen);

		} else {

			rewriteQuery(qd,query,querylen,
					liid,&columns,autoinccolumn);
		}

		// clean up
		columns.clearAndDelete();

	} else {
		copyQuery(qd,query,querylen);
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

void sqlrtrigger_replay::getQueryType(const char *query,
					uint32_t querylen,
					bool *isinsert,
					bool *isinsertselect,
					bool *isselectinto,
					bool *ismultiinsert) {
	*isinsert=false;
	*isinsertselect=false;
	*isselectinto=false;
	*ismultiinsert=false;

	const char	*start=cont->skipWhitespaceAndComments(query);

	// FIXME: assumes normalized query...

	if (querylen>12 && !charstring::compare(start,"insert into ",12)) {

		*isinsert=true;

		// FIXME: detect "INSERT ... SELECT ..."

		// FIXME: detect multi-insert

	} else if (querylen>7 && !charstring::compare(start,"select ",7)) {

		// FIXME: detect "SELECT ... INTO ..."
	}
}

void sqlrtrigger_replay::getColumns(const char *query,
					uint32_t querylen,
					linkedlist<char *> *columns,
					const char **autoinccolumn) {

	*autoinccolumn=NULL;

	// get table name...
	const char	*start=cont->skipWhitespaceAndComments(query)+12;
	const char	*end=charstring::findFirst(start,' ');
	if (!end) {
		return;
	}
	char	*table=charstring::duplicate(start,end-start);

	// get columns
	sqlrservercursor        *gclcur=cont->newCursor();
	if (cont->open(gclcur)) {

		bool	retval=false;
		if (cont->getListsByApiCalls()) {
			cont->setColumnListColumnMap(
					SQLRSERVERLISTFORMAT_MYSQL);
			retval=cont->getColumnList(gclcur,table,NULL);
		} else {
			const char	*q=
				cont->getColumnListQuery(table,false);
			// FIXME: clean up buffers to avoid SQL injection
			// FIXME: bounds checking
			char	*querybuffer=cont->getQueryBuffer(gclcur);
			charstring::printf(querybuffer,
					cont->getConfig()->getMaxQuerySize()+1,
					q,table);
			cont->setQueryLength(gclcur,
					charstring::length(querybuffer));
			retval=cont->prepareQuery(gclcur,
					cont->getQueryBuffer(gclcur),
					cont->getQueryLength(gclcur),
					false,false,false) &&
				cont->executeQuery(gclcur,
					false,false,false,false);
		}
		if (retval) {

			bool    error;
			while (cont->fetchRow(gclcur,&error)) {

				const char	*column;
				const char	*extra;
				uint64_t	fieldlength;
				bool		blob;
				bool		null;
				cont->getField(gclcur,0,&column,
						&fieldlength,&blob,&null);
				cont->getField(gclcur,8,&extra,
						&fieldlength,&blob,&null);

				char	*dup=charstring::duplicate(column);
				columns->append(dup);
				if (charstring::contains(extra,
							"auto_increment")) {
					*autoinccolumn=dup;
				}

				// FIXME: kludgy
				cont->nextRow(gclcur);
			}
		}
	}
	cont->closeResultSet(gclcur);
	cont->close(gclcur);
	cont->deleteCursor(gclcur);
}

void sqlrtrigger_replay::rewriteQuery(querydetails *qd,
						const char *query,
						uint32_t querylen,
						uint64_t liid,
						linkedlist<char *> *columns,
						const char *autoinccolumn) {
	stringbuffer	newquery;

	// did the query contain column names?

	// skip to the start of the query
	const char	*start=cont->skipWhitespaceAndComments(query);

	// skip to table name
	const char	*table=start+12;

	// skip to either "(" before columns or "values"
	const char	*colsstart=charstring::findFirst(table,' ')+1;

	// skip to first value (after ") values (")
	const char	*values=charstring::findFirst(colsstart,"values (")+8;

	// append up to the columns
	newquery.append(start,colsstart-start);

	char		**cols=NULL;
	uint64_t	colcount=0;

	if (*colsstart!='(') {

		// count values
		colcount=countValues(values);

		// create "cols" from "columns"
		cols=new char *[colcount];
		linkedlistnode<char *>	*node=columns->getFirst();
		for (uint64_t i=0; i<colcount; i++) {
			cols[i]=charstring::duplicate(node->getValue());
			node=node->getNext();
		}

	} else {

		// parse columns
		const char	*colsend=charstring::findFirst(colsstart,')');
		char		*colscopy=
				charstring::duplicate(colsstart+1,
							colsend-colsstart-1);
		charstring::split(colscopy,",",true,&cols,&colcount);
		delete[] colscopy;
	}

	// does the column list contain the autoincrement column?
	bool	columnsincludeautoinccolumn=false;
	for (uint64_t i=0; i<colcount; i++) {
		if (!charstring::compare(cols[i],autoinccolumn)) {
			columnsincludeautoinccolumn=true;
		}
	}

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

	// clean up
	for (uint64_t i=0; i<colcount; i++) {
		delete[] cols[i];
	}
	delete[] cols;

	// copy out the rewritten query
	copyQuery(qd,newquery.getString(),newquery.getStringLength());
}

uint64_t sqlrtrigger_replay::countValues(const char *values) {

	uint64_t	valuecount=0;
	const char	*c=values;
	char		prevc='\0';
	bool		inquotes=false;
	uint32_t	parens=0;
	for (;;) {

		// handle quotes...
		if (*c=='\'' && prevc!='\\' && prevc!='\'') {
			inquotes=!inquotes;
		}

		if (!inquotes) {

			// handle parentheses...
			if (*c=='(') {
				parens++;
			} else if (parens && *c==')') {
				parens--;
			} else {
				// bump the value count if we found a comma
				if (*c==',') {
					valuecount++;
				}
			}
		}

		// keep going
		prevc=*c;
		c++;
		if (!inquotes && !parens && *c==')') {
			valuecount++;
			break;
		}
	}

	return valuecount;
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

		// handle quotes...
		if (*c=='\'' && prevc!='\\' && prevc!='\'') {
			inquotes=!inquotes;
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

bool sqlrtrigger_replay::replayLog(sqlrservercursor *sqlrcur) {
//debug=true;

	bool	retval=true;

	// we're replaying the log
	inreplay=true;

	uint32_t	sec=0;
	uint32_t	usec=0;
	uint32_t	retry=0;
	while (replayCondition(sqlrcur)) {

		// delay before trying again...
		// delay a little longer before each retry, up to 10 seconds
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
				stdoutput.printf("delay %d sec, %d usec...",
								sec,usec);
			}
			snooze::microsnooze(sec,usec);
		}


		if (debug) {
			stdoutput.printf("replay {\n");
		}

		if (debug) {
			stdoutput.printf("	triggering query:\n%.*s\n",
						sqlrcur->getQueryLength(),
						sqlrcur->getQueryBuffer());
		}

		// clear the error
		cont->clearError();
		cont->clearError(sqlrcur);

		/*if (replaytx) {

			// roll back the current transaction
			if (debug) {
				stdoutput.printf("	rollback {\n");
			}
			if (!cont->rollback()) {
				if (debug) {
					stdoutput.printf("	error\n");
					stdoutput.printf("}\n");
				}
				logpool.clear();
				log.clearAndDelete();
				return false;
			}

			// start a new transaction
			if (debug) {
				stdoutput.printf("	}\n");
				stdoutput.printf("	begin {\n");
			}
			if (!cont->begin()) {
				if (debug) {
					stdoutput.printf("	error\n");
					stdoutput.printf("}\n");
				}
				logpool.clear();
				log.clearAndDelete();
				return false;
			}
			if (debug) {
				stdoutput.printf("	}\n");
			}
		}*/

		// get the bind pool
		memorypool	*pool=cont->getBindPool(sqlrcur);

		// replay the log
		retval=true;
		for (linkedlistnode<querydetails *> *node=log.getFirst();
						node; node=node->getNext()) {

			// get the query details
			querydetails	*qd=node->getValue();
		
			// prepare the query
			if (debug) {
				stdoutput.printf("	prepare query {\n");
				stdoutput.printf("		query:\n%.*s\n",
							qd->querylen,qd->query);
			}
			if (!cont->prepareQuery(sqlrcur,
						qd->query,qd->querylen)) {
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
			linkedlistnode<sqlrserverbindvar *>	*inbindnode=
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
			linkedlistnode<sqlrserverbindvar *>	*outbindnode=
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
			linkedlistnode<sqlrserverbindvar *>	*inoutbindnode=
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
				if (debug) {
					stdoutput.printf(
						"		"
						"execute error: %.*s\n",
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
		}

		if (debug) {
			stdoutput.printf("}\n");
		}

		// bump retry count
		retry++;

		// bail if we've tried too many times already
		if (maxretries && retry>maxretries) {
			break;
		}
	}

	if (replaytx && !retval) {
		// clear the log and roll back on error
		cont->rollback();
		logpool.clear();
		log.clearAndDelete();
	}

	// we're no longer replaying the log
	inreplay=false;

//debug=false;
	return retval;
}

bool sqlrtrigger_replay::replayCondition(sqlrservercursor *sqlrcur) {

	// did we get a replay condition?
	bool	found=false;
	for (linkedlistnode<condition *> *node=conditions.getFirst();
						node; node=node->getNext()) {

		condition	*val=node->getValue();

		if (val->cond==CONDITION_ERROR) {

			// FIXME: error buffer might not be terminated
			if (charstring::contains(
				sqlrcur->getErrorBuffer(),val->error)) {
				found=true;
				if (debug) {
					stdoutput.printf(
						"replay condition "
						"detected {\n");
					stdoutput.printf("	"
						"pattern: %s\n",val->error);
					stdoutput.printf("	"
						"error string: %.*s\n",
						sqlrcur->getErrorLength(),
						sqlrcur->getErrorBuffer());
					stdoutput.printf("}\n");
				}
			}

		} else if (val->cond==CONDITION_ERRORCODE) {

			if (sqlrcur->getErrorNumber()==val->errorcode) {
				found=true;
				if (debug) {
					stdoutput.printf(
						"replay condition "
						"detected {\n");
					stdoutput.printf("	"
						"error code: %d\n",
						val->errorcode);
					stdoutput.printf("}\n");
				}
			}
		}
	}
	return found;
}

void sqlrtrigger_replay::endTransaction(bool commit) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return;
	}

	// bail if we're not replaying transactions queries...
	if (!replaytx) {
		return;
	}

	logpool.clear();
	log.clearAndDelete();
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_replay(
						sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_replay(cont,ts,parameters);
	}
}
