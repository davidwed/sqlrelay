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
		bool		replaytx;
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
		bool	replay(sqlrservercursor *sqlrcur, bool replaytx);
		bool	replayCondition(sqlrservercursor *sqlrcur,
							bool *replaytx,
							bool indent);


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

		uint32_t	maxretries;

		linkedlist<querydetails *>	log;
		linkedlist<condition *>		conditions;
		memorypool			logpool;

		bool	inreplay;

		bool	wasintx;
};

sqlrtrigger_replay::sqlrtrigger_replay(sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	this->cont=cont;

	debug=cont->getConfig()->getDebugTriggers();
debug=true;

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

		conditions.append(c);
	}

	inreplay=false;

	wasintx=false;
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

	// bail if we're currently replaying the log...
	if (inreplay) {
		return true;
	}

	// If we're not in a transaction, we only need
	// to log the current query.  Clear the log.
	if (!cont->inTransaction()) {
		logpool.clear();
		log.clearAndDelete();
	}

	// If we weren't in a transaction, but are now,
	// then we also need to clear the log.
	if (cont->inTransaction() && !wasintx) {
		logpool.clear();
		log.clearAndDelete();
		wasintx=true;
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

		// end-of-values condition
		if (!inquotes && !parens && *c==')') {
			valuecount++;
			break;
		}

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

bool sqlrtrigger_replay::replay(sqlrservercursor *sqlrcur, bool replaytx) {

	// we're replaying
	inreplay=true;

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
	linkedlistnode<querydetails *> *current=
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
		log.clearAndDelete();
	}

	// we're no longer replaying the log
	inreplay=false;

	return retval;
}

bool sqlrtrigger_replay::replayCondition(sqlrservercursor *sqlrcur,
							bool *replaytx,
							bool indent) {

	// did we get a replay condition?
	for (linkedlistnode<condition *> *node=conditions.getFirst();
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
				return true;
			}
		}
	}
	return false;
}

void sqlrtrigger_replay::endTransaction(bool commit) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return;
	}

	logpool.clear();
	log.clearAndDelete();

	wasintx=false;
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
