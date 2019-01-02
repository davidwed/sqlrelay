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
					bool success);

		void	endTransaction(bool commit);

	private:
		bool	logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);
		bool	replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur);


		void	copyInputBind(sqlrserverbindvar *source,
						sqlrserverbindvar *dest);
		void	copyOutputBind(sqlrserverbindvar *source,
						sqlrserverbindvar *dest);
		void	copyInputOutputBind(sqlrserverbindvar *source,
						sqlrserverbindvar *dest);

		linkedlist<querydetails *>	log;

		bool	inreplay;

		uint32_t	errorcode;
};

sqlrtrigger_deadlock_replay::sqlrtrigger_deadlock_replay(
					sqlrservercontroller *cont,
					sqlrtriggers *ts,
					domnode *parameters) :
					sqlrtrigger(cont,ts,parameters) {
	inreplay=false;
	errorcode=charstring::toInteger(
			parameters->getAttributeValue("errorcode"));
}

bool sqlrtrigger_deadlock_replay::run(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						bool before,
						bool success) {
	return (before)?logQuery(sqlrcon,sqlrcur):replayLog(sqlrcon,sqlrcur);
}

bool sqlrtrigger_deadlock_replay::logQuery(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// bail if we're currently replaying the log...
	if (inreplay) {
		return true;
	}

	// log the query...

	memorypool	*pool=sqlrcon->cont->getPerTransactionMemoryPool();
	querydetails	*qd=new querydetails;

	// copy the query itself
	qd->querylen=sqlrcur->getQueryLength();
	qd->query=(char *)pool->allocate(qd->querylen+1);

	// copy input binds
	uint16_t		incount=sqlrcur->getInputBindCount();
	sqlrserverbindvar	*invars=sqlrcur->getInputBinds();
	for (uint16_t i=0; i<incount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyInputBind(&(invars[i]),bv);
		qd->inbindvars.append(bv);
	}
	
	// copy output binds
	uint16_t		outcount=sqlrcur->getOutputBindCount();
	sqlrserverbindvar	*outvars=sqlrcur->getOutputBinds();
	for (uint16_t i=0; i<outcount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyOutputBind(&(outvars[i]),bv);
		qd->outbindvars.append(bv);
	}

	// copy input-output binds
	uint16_t		inoutcount=sqlrcur->getInputOutputBindCount();
	sqlrserverbindvar	*inoutvars=sqlrcur->getInputOutputBinds();
	for (uint16_t i=0; i<inoutcount; i++) {
		sqlrserverbindvar	*bv=new sqlrserverbindvar;
		copyInputOutputBind(&(inoutvars[i]),bv);
		qd->inoutbindvars.append(bv);
	}

	// log copied query and binds
	log.append(qd);
	
	return true;
}

void sqlrtrigger_deadlock_replay::copyInputBind(
					sqlrserverbindvar *source,
					sqlrserverbindvar *dest) {
	// FIXME: implement this...
}

void sqlrtrigger_deadlock_replay::copyOutputBind(
					sqlrserverbindvar *source,
					sqlrserverbindvar *dest) {
	// FIXME: implement this...
}

void sqlrtrigger_deadlock_replay::copyInputOutputBind(
					sqlrserverbindvar *source,
					sqlrserverbindvar *dest) {
	// FIXME: implement this...
}

bool sqlrtrigger_deadlock_replay::replayLog(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur) {

	// bail if we didn't get a deadlock...
	if (sqlrcur->getErrorNumber()!=errorcode) {
		return true;
	}

	// replay the log...
	inreplay=true;

	// ...

	inreplay=false;

	return true;
}

void sqlrtrigger_deadlock_replay::endTransaction(bool commit) {
	// clear the log...
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
