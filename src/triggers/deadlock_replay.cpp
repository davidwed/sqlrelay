// Copyright (c) 1999-2018  David Muse
// All rights reserved

#include <sqlrelay/sqlrserver.h>

class querydetails {
	public:
		char		*query;
		uint32_t	querylen;
		linkedlist<sqlrserverbindvar *>	inbindvars;
		linkedlist<sqlrserverbindvar *>	outbindvars;
		linkedlist<sqlrserverbindvar *>	inoutbindvars;
};

class SQLRSERVER_DLLSPEC sqlrtrigger_deadlock_replay : public sqlrtrigger {
	public:
			sqlrtrigger_deadlock_replay(
					sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					bool before,
					bool *success);

		void	endTransaction(bool commit);

	private:
		bool	logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool	replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);


		void	copyBind(memorypool *pool,
					sqlrserverbindvar *dest,
					sqlrserverbindvar *source);

		sqlrservercontroller	*cont;

		bool		debug;
		const char	*error;
		uint32_t	errorcode;

		linkedlist<querydetails *>	log;

		bool	inreplay;
};

sqlrtrigger_deadlock_replay::sqlrtrigger_deadlock_replay(
					sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	this->cont=cont;

	debug=cont->getConfig()->getDebugTriggers();

	const char	*err=parameters->getAttributeValue("error");
	error=NULL;
	if (charstring::isNumber(err)) {
		errorcode=charstring::toInteger(err);
	} else {
		error=err;
	}

	inreplay=false;
}

bool sqlrtrigger_deadlock_replay::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool *success) {
	if (before) {
		return logQuery(sqlrcon,sqlrcur);
	} else {
		*success=replayLog(sqlrcon,sqlrcur);
		return *success;
	}
}

bool sqlrtrigger_deadlock_replay::logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return true;
	}

	// bail if we're not in a transaction
	if (!cont->inTransaction()) {
		if (debug) {
			stdoutput.printf("not in a trasaction\n");
		}
		return true;
	}

	// log the query...

	querydetails	*qd=new querydetails;
	memorypool	*pool=cont->getPerTransactionMemoryPool();

	if (debug) {
		stdoutput.printf("log {\n");
	}

	// copy the query itself (make sure to null terminate)
	qd->querylen=sqlrcur->getQueryLength();
	qd->query=(char *)pool->allocate(qd->querylen+1);
	charstring::copy(qd->query,sqlrcur->getQueryBuffer());

	if (debug) {
		stdoutput.printf("	query:\n%.*s\n",qd->querylen,qd->query);
	}

	// copy in input binds
	uint16_t		incount=sqlrcur->getInputBindCount();
	sqlrserverbindvar	*invars=sqlrcur->getInputBinds();
	if (debug && incount) {
		stdoutput.printf("	input binds {\n");
	}
	for (uint16_t i=0; i<incount; i++) {
		if (debug) {
			stdoutput.printf("		%.*s\n",
						invars[i].variablesize,
						invars[i].variable);
		}
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(pool,bv,&(invars[i]));
		qd->inbindvars.append(bv);
	}
	if (debug && incount) {
		stdoutput.printf("	}\n");
	}
	
	// copy in output binds
	uint16_t		outcount=sqlrcur->getOutputBindCount();
	sqlrserverbindvar	*outvars=sqlrcur->getOutputBinds();
	if (debug && outcount) {
		stdoutput.printf("	output binds {\n");
	}
	for (uint16_t i=0; i<outcount; i++) {
		if (debug) {
			stdoutput.printf("		%.*s\n",
						outvars[i].variablesize,
						outvars[i].variable);
		}
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(pool,bv,&(outvars[i]));
		qd->outbindvars.append(bv);
	}
	if (debug && outcount) {
		stdoutput.printf("	}\n");
	}

	// copy in input-output binds
	uint16_t		inoutcount=sqlrcur->getInputOutputBindCount();
	sqlrserverbindvar	*inoutvars=sqlrcur->getInputOutputBinds();
	if (debug && inoutcount) {
		stdoutput.printf("	input-output binds {\n");
	}
	for (uint16_t i=0; i<inoutcount; i++) {
		if (debug) {
			stdoutput.printf("		%.*s\n",
						inoutvars[i].variablesize,
						inoutvars[i].variable);
		}
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyBind(pool,bv,&(inoutvars[i]));
		qd->inoutbindvars.append(bv);
	}
	if (debug && inoutcount) {
		stdoutput.printf("	}\n");
	}

	// log copied query and binds
	log.append(qd);

	if (debug) {
		stdoutput.printf("}\n");
	}
	
	return true;
}

void sqlrtrigger_deadlock_replay::copyBind(
					memorypool *pool,
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

bool sqlrtrigger_deadlock_replay::replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// bail if we didn't get a deadlock...
	if (error) {
		// FIXME: error buffer might not be terminated
		if (!charstring::contains(sqlrcur->getErrorBuffer(),error)) {
			return true;
		}
		if (debug) {
			stdoutput.printf("deadlock detected {\n");
			stdoutput.printf("	pattern: %s\n",error);
			stdoutput.printf("	error string: %.*s\n",
						sqlrcur->getErrorLength(),
						sqlrcur->getErrorBuffer());
			stdoutput.printf("}\n");
		}
	} else {
		if (sqlrcur->getErrorNumber()!=errorcode) {
			return true;
		}
		if (debug) {
			stdoutput.printf("deadlock detected {\n");
			stdoutput.printf("	error code: %d\n",errorcode);
			stdoutput.printf("}\n");
		}
	}

	if (debug) {
		stdoutput.printf("replay log {\n");
	}

	// we're replaying the log
	inreplay=true;

	// clear the error
	cont->clearError();
	cont->clearError(sqlrcur);

	// roll back the current transaction
	if (debug) {
		stdoutput.printf("	rollback {\n");
	}
	if (!cont->rollback()) {
		if (debug) {
			stdoutput.printf("	error\n");
			stdoutput.printf("}\n");
		}
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
		log.clearAndDelete();
		return false;
	}
	if (debug) {
		stdoutput.printf("	}\n");
	}

	// get the bind pool
	memorypool	*pool=cont->getBindPool(sqlrcur);

	// replay the log
	bool	retval=true;
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
		if (!cont->prepareQuery(sqlrcur,qd->query,qd->querylen)) {
			if (debug) {
				stdoutput.printf("		"
						"prepare error...\n");
				stdoutput.printf("	}\n");
			}
			retval=false;
			break;
		}
		if (debug) {
			stdoutput.printf("	}\n");
		}

		// copy out input binds
		uint16_t		incount=qd->inbindvars.getLength();
		sqlrcur->setInputBindCount(incount);
		sqlrserverbindvar	*invars=sqlrcur->getInputBinds();
		if (debug && incount) {
			stdoutput.printf("	input binds {\n");
		}
		linkedlistnode<sqlrserverbindvar *>	*inbindnode=
						qd->inbindvars.getFirst();
		for (uint16_t i=0; i<incount; i++) {
			sqlrserverbindvar	*bv=inbindnode->getValue();
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
		uint16_t		outcount=qd->outbindvars.getLength();
		sqlrcur->setInputBindCount(outcount);
		sqlrserverbindvar	*outvars=sqlrcur->getOutputBinds();
		if (debug && outcount) {
			stdoutput.printf("	output binds {\n");
		}
		linkedlistnode<sqlrserverbindvar *>	*outbindnode=
						qd->outbindvars.getFirst();
		for (uint16_t i=0; i<outcount; i++) {
			sqlrserverbindvar	*bv=outbindnode->getValue();
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
			stdoutput.printf("	input-output binds {\n");
		}
		linkedlistnode<sqlrserverbindvar *>	*inoutbindnode=
						qd->inoutbindvars.getFirst();
		for (uint16_t i=0; i<inoutcount; i++) {
			sqlrserverbindvar	*bv=inoutbindnode->getValue();
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
				stdoutput.printf("		"
						"execute error...\n");
				stdoutput.printf("	}\n");
			}
			retval=false;
			break;
		}
		if (debug) {
			stdoutput.printf("	}\n");
		}
	}

	if (!retval) {
		// clear the log and roll back on error
		cont->rollback();
		log.clearAndDelete();
	}

	// we're no longer replaying the log
	inreplay=false;

	if (debug) {
		stdoutput.printf("}\n");
	}

	return retval;
}

void sqlrtrigger_deadlock_replay::endTransaction(bool commit) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return;
	}

	if (debug) {
		stdoutput.printf("clear log {\n");
	}
	log.clearAndDelete();
	if (debug) {
		stdoutput.printf("}\n");
	}
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrtrigger	*new_sqlrtrigger_deadlock_replay(
						sqlrservercontroller *cont,
						sqlrtriggers *ts,
						domnode *parameters) {

		return new sqlrtrigger_deadlock_replay(cont,ts,parameters);
	}
}
