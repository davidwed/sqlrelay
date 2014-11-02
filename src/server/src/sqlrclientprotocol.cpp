// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclientprotocol.h>
#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrcursor.h>

#include <rudiments/bytestring.h>
#include <rudiments/signalclasses.h>
#include <rudiments/datetime.h>

#include <datatypes.h>
#include <defines.h>

sqlrclientprotocol::sqlrclientprotocol(sqlrcontroller_svr *cont,
					sqlrconnection_svr *conn) :
					sqlrprotocol(cont,conn) {
	idleclienttimeout=cont->cfgfl->getIdleClientTimeout();
	maxclientinfolength=cont->cfgfl->getMaxClientInfoLength();
	maxquerysize=cont->cfgfl->getMaxQuerySize();
	maxbindcount=cont->cfgfl->getMaxBindCount();
	maxbindnamelength=cont->cfgfl->getMaxBindNameLength();
	maxstringbindvaluelength=cont->cfgfl->getMaxStringBindValueLength();
	maxlobbindvaluelength=cont->cfgfl->getMaxLobBindValueLength();
	bindpool=new memorypool(512,128,100);
	maxerrorlength=cont->cfgfl->getMaxErrorLength();
}

sqlrclientprotocol::~sqlrclientprotocol() {
	delete bindpool;
}

sqlrclientexitstatus_t sqlrclientprotocol::clientSession() {

	// During each session, the client will send a series of commands.
	// The session ends when the client ends it or when certain commands
	// fail.
	bool			loop=true;
	bool			endsession=true;
	sqlrclientexitstatus_t	status=SQLRCLIENTEXITSTATUS_ERROR;
	uint16_t		command;
	do {

		// get a command from the client
		if (!getCommand(&command)) {
			break;
		}

		// get the command start time
		datetime	dt;
		dt.getSystemDateAndTime();

		// these commands are all handled at the connection level
		if (command==AUTHENTICATE) {
			cont->incrementAuthenticateCount();
			if (authenticateCommand()) {
				cont->sessionStartQueries();
				continue;
			}
			endsession=false;
			break;
		} else if (command==SUSPEND_SESSION) {
			cont->incrementSuspendSessionCount();
			suspendSessionCommand();
			status=SQLRCLIENTEXITSTATUS_SUSPENDED_SESSION;
			endsession=false;
			break;
		} else if (command==END_SESSION) {
			cont->incrementEndSessionCount();
			status=SQLRCLIENTEXITSTATUS_ENDED_SESSION;
			break;
		} else if (command==PING) {
			cont->incrementPingCount();
			pingCommand();
			continue;
		} else if (command==IDENTIFY) {
			cont->incrementIdentifyCount();
			identifyCommand();
			continue;
		} else if (command==AUTOCOMMIT) {
			cont->incrementAutocommitCount();
			autoCommitCommand();
			continue;
		} else if (command==BEGIN) {
			cont->incrementBeginCount();
			beginCommand();
			continue;
		} else if (command==COMMIT) {
			cont->incrementCommitCount();
			commitCommand();
			continue;
		} else if (command==ROLLBACK) {
			cont->incrementRollbackCount();
			rollbackCommand();
			continue;
		} else if (command==DBVERSION) {
			cont->incrementDbVersionCount();
			dbVersionCommand();
			continue;
		} else if (command==BINDFORMAT) {
			cont->incrementBindFormatCount();
			bindFormatCommand();
			continue;
		} else if (command==SERVERVERSION) {
			cont->incrementServerVersionCount();
			serverVersionCommand();
			continue;
		} else if (command==SELECT_DATABASE) {
			cont->incrementSelectDatabaseCount();
			selectDatabaseCommand();
			continue;
		} else if (command==GET_CURRENT_DATABASE) {
			cont->incrementGetCurrentDatabaseCount();
			getCurrentDatabaseCommand();
			continue;
		} else if (command==GET_LAST_INSERT_ID) {
			cont->incrementGetLastInsertIdCount();
			getLastInsertIdCommand();
			continue;
		} else if (command==DBHOSTNAME) {
			cont->incrementDbHostNameCount();
			dbHostNameCommand();
			continue;
		} else if (command==DBIPADDRESS) {
			cont->incrementDbIpAddressCount();
			dbIpAddressCommand();
			continue;
		}

		// For the rest of the commands,
		// the client will request a cursor
		sqlrcursor_svr	*cursor=getCursor(command);
		if (!cursor) {
			// Don't worry about reporting that a cursor wasn't
			// available for abort-result-set commands. Those
			// commands don't look for a response from the server
			// and it doesn't matter if a non-existent result set
			// was aborted.
			if (command!=ABORT_RESULT_SET) {
				noAvailableCursors(command);
			}
			continue;
		}

		// keep track of the command start time
		cursor->setCommandStart(dt.getSeconds(),dt.getMicroseconds());

		// these commands are all handled at the cursor level
		if (command==NEW_QUERY) {
			cont->incrementNewQueryCount();
			loop=newQueryCommand(cursor);
		} else if (command==REEXECUTE_QUERY) {
			cont->incrementReexecuteQueryCount();
			loop=reExecuteQueryCommand(cursor);
		} else if (command==FETCH_FROM_BIND_CURSOR) {
			cont->incrementFetchFromBindCursorCount();
			loop=fetchFromBindCursorCommand(cursor);
		} else if (command==FETCH_RESULT_SET) {
			cont->incrementFetchResultSetCount();
			loop=fetchResultSetCommand(cursor);
		} else if (command==ABORT_RESULT_SET) {
			cont->incrementAbortResultSetCount();
			abortResultSetCommand(cursor);
		} else if (command==SUSPEND_RESULT_SET) {
			cont->incrementSuspendResultSetCount();
			suspendResultSetCommand(cursor);
		} else if (command==RESUME_RESULT_SET) {
			cont->incrementResumeResultSetCount();
			loop=resumeResultSetCommand(cursor);
		} else if (command==GETDBLIST) {
			cont->incrementGetDbListCount();
			loop=getDatabaseListCommand(cursor);
		} else if (command==GETTABLELIST) {
			cont->incrementGetTableListCount();
			loop=getTableListCommand(cursor);
		} else if (command==GETCOLUMNLIST) {
			cont->incrementGetColumnListCount();
			loop=getColumnListCommand(cursor);
		} else if (command==GET_QUERY_TREE) {
			cont->incrementGetQueryTreeCount();
			loop=getQueryTreeCommand(cursor);
		} else {
			loop=false;
		}

		// get the command end-time
		dt.getSystemDateAndTime();
		cursor->setCommandEnd(dt.getSeconds(),dt.getMicroseconds());

		// log query-related commands
		// FIXME: this won't log triggers
		if (command==NEW_QUERY ||
			command==REEXECUTE_QUERY ||
			command==FETCH_FROM_BIND_CURSOR) {
			cont->logQuery(cursor);
		}

	} while (loop);

	// end the session if necessary
	if (endsession) {
		cont->endSession();
	}

	// return the exit status
	return status;
}

bool sqlrclientprotocol::getCommand(uint16_t *command) {

	cont->logDebugMessage("getting command...");

	cont->updateState(GET_COMMAND);

	// get the command
	ssize_t	result=clientsock->read(command,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {

		// Return false but don't consider it an error if we get a
		// timeout or a 0 (meaning that the client closed the socket)
		// as either would be natural to do here.
		if (result!=RESULT_TIMEOUT && result!=0) {
			cont->logClientProtocolError(
				NULL,"get command failed",result);
		}

		*command=NO_COMMAND;
		return false;
	}

	debugstr.clear();
	debugstr.append("command: ")->append(*command);
	cont->logDebugMessage(debugstr.getString());

	cont->logDebugMessage("done getting command");
	return true;
}

sqlrcursor_svr *sqlrclientprotocol::getCursor(uint16_t command) {

	cont->logDebugMessage("getting a cursor...");

	// does the client need a cursor or does it already have one
	uint16_t	neednewcursor=DONT_NEED_NEW_CURSOR;
	if (command==NEW_QUERY ||
		command==GETDBLIST ||
		command==GETTABLELIST ||
		command==GETCOLUMNLIST ||
		command==ABORT_RESULT_SET ||
		command==GET_QUERY_TREE) {
		ssize_t	result=clientsock->read(&neednewcursor,
						idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->logClientProtocolError(NULL,
					"get cursor failed: "
					"failed to get whether client "
					"needs  new cursor or not",result);
			return NULL;
		}
	}

	sqlrcursor_svr	*cursor=NULL;

	if (neednewcursor==DONT_NEED_NEW_CURSOR) {

		// which cursor is the client requesting?
		uint16_t	id;
		ssize_t		result=clientsock->read(&id,
						idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->logClientProtocolError(NULL,
					"get cursor failed: "
					"failed to get cursor id",result);
			return NULL;
		}

		// get the requested cursor
		cursor=cont->getCursorById(id);

		if (cursor) {
			// mark this as an old cursor being reused
			cont->incrementTimesCursorReused();
		} else {
			debugstr.clear();
			debugstr.append("get cursor failed: "
					"client requested an invalid cursor: ");
			debugstr.append(id);
			cont->logClientProtocolError(NULL,
					debugstr.getString(),1);
			return NULL;
		}

	} else {

		// find an available cursor
		cursor=cont->findAvailableCursor();

		if (cursor) {
			// mark this as a new cursor being used
			cont->incrementTimesNewCursorUsed();
		}
	}

	cont->logDebugMessage("done getting a cursor");
	return cursor;
}

void sqlrclientprotocol::noAvailableCursors(uint16_t command) {

	// If no cursor was available, the client
	// cound send an entire query and bind vars
	// before it reads the error and closes the
	// socket.  We have to absorb all of that
	// data.  We shouldn't just loop forever
	// though, that would provide a point of entry
	// for a DOS attack.  We'll read the maximum
	// number of bytes that could be sent.
	uint32_t	size=(
				// query size and query
				sizeof(uint32_t)+maxquerysize+
				// input bind var count
				sizeof(uint16_t)+
				// input bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// get column info
				sizeof(uint16_t)+
				// skip/fetch
				2*sizeof(uint32_t));

	clientsock->useNonBlockingMode();
	unsigned char	*dummy=new unsigned char[size];
	clientsock->read(dummy,size,idleclienttimeout,0);
	clientsock->useBlockingMode();
	delete[] dummy;

	// indicate that an error has occurred
	clientsock->write((uint16_t)ERROR_OCCURRED);

	// send the error code
	clientsock->write((uint64_t)SQLR_ERROR_NOCURSORS);

	// send the error itself
	uint16_t	len=charstring::length(SQLR_ERROR_NOCURSORS_STRING);
	clientsock->write(len);
	clientsock->write(SQLR_ERROR_NOCURSORS_STRING,len);
	clientsock->flushWriteBuffer(-1,-1);
}

bool sqlrclientprotocol::authenticateCommand() {

	cont->logDebugMessage("authenticate");

	// get the user/password from the client and authenticate
	if (getUserFromClient() &&
		getPasswordFromClient() &&
		cont->authenticate(userbuffer,passwordbuffer)) {
		return true;
	}

	// indicate that an error has occurred
	clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	clientsock->write((uint64_t)SQLR_ERROR_AUTHENTICATIONERROR);
	clientsock->write((uint16_t)charstring::length(
				SQLR_ERROR_AUTHENTICATIONERROR_STRING));
	clientsock->write(SQLR_ERROR_AUTHENTICATIONERROR_STRING);
	clientsock->flushWriteBuffer(-1,-1);
	conn->endSession();
	return false;
}

bool sqlrclientprotocol::getUserFromClient() {
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get user size",result);
		return false;
	}
	if (size>=sizeof(userbuffer)) {
		debugstr.clear();
		debugstr.append("authentication failed: user size too long: ");
		debugstr.append(size);
		cont->logClientConnectionRefused(debugstr.getString());
		return false;
	}
	result=clientsock->read(userbuffer,size,idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get user",result);
		return false;
	}
	userbuffer[size]='\0';
	return true;
}

bool sqlrclientprotocol::getPasswordFromClient() {
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get password size",result);
		return false;
	}
	if (size>=sizeof(passwordbuffer)) {
		debugstr.clear();
		debugstr.append("authentication failed: "
				"password size too long: ");
		debugstr.append(size);
		cont->logClientConnectionRefused(debugstr.getString());
		return false;
	}
	result=clientsock->read(passwordbuffer,size,idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get password",result);
		return false;
	}
	passwordbuffer[size]='\0';
	return true;
}

void sqlrclientprotocol::suspendSessionCommand() {

	cont->logDebugMessage("suspending session...");

	// suspend the session
	const char	*unixsocketname=NULL;
	uint16_t	inetportnumber=0;
	cont->suspendSession(&unixsocketname,&inetportnumber);
	uint16_t	unixsocketsize=charstring::length(unixsocketname);

	// pass the socket info to the client
	cont->logDebugMessage("passing socket info to client...");
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write(unixsocketsize);
	if (unixsocketsize) {
		clientsock->write(unixsocketname,unixsocketsize);
	}
	clientsock->write(inetportnumber);
	clientsock->flushWriteBuffer(-1,-1);
	cont->logDebugMessage("done passing socket info to client");

	cont->logDebugMessage("done suspending session");
}

void sqlrclientprotocol::pingCommand() {
	cont->logDebugMessage("ping");
	bool	pingresult=conn->ping();
	if (pingresult) {
		cont->logDebugMessage("ping succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("ping failed");
		returnError(!conn->getLiveConnection());
	}
	if (!pingresult) {
		cont->reLogIn();
	}
}

void sqlrclientprotocol::identifyCommand() {

	cont->logDebugMessage("identify");

	// get the identification
	const char	*ident=conn->identify();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	idlen=charstring::length(ident);
	clientsock->write(idlen);
	clientsock->write(ident,idlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrclientprotocol::autoCommitCommand() {
	cont->logDebugMessage("autocommit...");
	bool	on;
	ssize_t	result=clientsock->read(&on,idleclienttimeout,0);
	if (result!=sizeof(bool)) {
		cont->logClientProtocolError(NULL,
				"get autocommit failed: "
				"failed to get autocommit setting",result);
		return;
	}
	bool	success=false;
	if (on) {
		cont->logDebugMessage("autocommit on");
		success=cont->autoCommitOn();
	} else {
		cont->logDebugMessage("autocommit off");
		success=cont->autoCommitOff();
	}
	if (success) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->getLiveConnection());
	}
}

void sqlrclientprotocol::beginCommand() {
	cont->logDebugMessage("begin...");
	if (cont->begin()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->getLiveConnection());
	}
}

void sqlrclientprotocol::commitCommand() {
	cont->logDebugMessage("commit...");
	if (cont->commit()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->getLiveConnection());
	}
}

void sqlrclientprotocol::rollbackCommand() {
	cont->logDebugMessage("rollback...");
	if (cont->rollback()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->getLiveConnection());
	}
}

void sqlrclientprotocol::dbVersionCommand() {

	cont->logDebugMessage("db version");

	// get the db version
	const char	*dbversion=conn->dbVersion();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	dbvlen=charstring::length(dbversion);
	clientsock->write(dbvlen);
	clientsock->write(dbversion,dbvlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrclientprotocol::bindFormatCommand() {

	cont->logDebugMessage("bind format");

	// get the bind format
	const char	*bf=conn->bindFormat();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	bflen=charstring::length(bf);
	clientsock->write(bflen);
	clientsock->write(bf,bflen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrclientprotocol::serverVersionCommand() {

	cont->logDebugMessage("server version");

	// get the server version
	const char	*svrversion=SQLR_VERSION;

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	svrvlen=charstring::length(svrversion);
	clientsock->write(svrvlen);
	clientsock->write(svrversion,svrvlen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrclientprotocol::selectDatabaseCommand() {

	cont->logDebugMessage("select database");

	// get length of db parameter
	uint32_t	dblen;
	ssize_t		result=clientsock->read(&dblen,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		clientsock->write(false);
		cont->logClientProtocolError(NULL,
				"select database failed: "
				"failed to get db length",result);
		return;
	}

	// bounds checking
	if (dblen>maxquerysize) {
		clientsock->write(false);
		debugstr.clear();
		debugstr.append("select database failed: "
				"client sent bad db length: ");
		debugstr.append(dblen);
		cont->logClientProtocolError(NULL,debugstr.getString(),1);
		return;
	}

	// read the db parameter into the buffer
	char	*db=new char[dblen+1];
	if (dblen) {
		result=clientsock->read(db,dblen,idleclienttimeout,0);
		if ((uint32_t)result!=dblen) {
			clientsock->write(false);
			clientsock->flushWriteBuffer(-1,-1);
			delete[] db;
			cont->logClientProtocolError(NULL,
				"select database failed: "
				"failed to get database name",result);
			return;
		}
	}
	db[dblen]='\0';
	
	// Select the db and send back the result.
	if (cont->selectDatabase(db)) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		returnError(!conn->getLiveConnection());
	}

	delete[] db;

	return;
}

void sqlrclientprotocol::getCurrentDatabaseCommand() {

	cont->logDebugMessage("get current database");

	// get the current database
	char	*currentdb=conn->getCurrentDatabase();

	// send it to the client
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	currentdbsize=charstring::length(currentdb);
	clientsock->write(currentdbsize);
	clientsock->write(currentdb,currentdbsize);
	clientsock->flushWriteBuffer(-1,-1);

	// clean up
	delete[] currentdb;
}

void sqlrclientprotocol::getLastInsertIdCommand() {
	cont->logDebugMessage("getting last insert id...");
	uint64_t	id;
	if (conn->getLastInsertId(&id)) {
		cont->logDebugMessage("get last insert id succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->write(id);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("get last insert id failed");
		returnError(!conn->getLiveConnection());
	}
}

void sqlrclientprotocol::dbHostNameCommand() {

	cont->logDebugMessage("getting db host name");

	const char	*hostname=conn->dbHostName();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	hostnamelen=charstring::length(hostname);
	clientsock->write(hostnamelen);
	clientsock->write(hostname,hostnamelen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrclientprotocol::dbIpAddressCommand() {

	cont->logDebugMessage("getting db host name");

	const char	*ipaddress=conn->dbIpAddress();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	ipaddresslen=charstring::length(ipaddress);
	clientsock->write(ipaddresslen);
	clientsock->write(ipaddress,ipaddresslen);
	clientsock->flushWriteBuffer(-1,-1);
}

bool sqlrclientprotocol::newQueryCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("new query");

	cont->logDebugMessage("handling query...");

	cursor=cont->initQueryOrBindCursor(cursor,false,false,true);

	// get the client info and query from the client
	bool	success=(getClientInfo(cursor) && getQuery(cursor));

	// do we need to use a custom query handler for this query?
	if (success) {
		sqlrcursor_svr	*originalcursor=cursor;
		cursor=cont->useCustomQueryHandler(cursor);
		cont->updateState((cursor!=originalcursor)?
					PROCESS_CUSTOM:PROCESS_SQL);
	}

	// get binds and whether to get column info
	if (success) {
		success=(getInputBinds(cursor) &&
				getOutputBinds(cursor) &&
				getSendColumnInfo());
	}

	if (success) {
		return processQueryOrBindCursor(cursor,false,false);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cursor->getErrorNumber()) {
		returnError(cursor,true);
	}
	cont->logDebugMessage("failed to handle query");
	return false;
}

bool sqlrclientprotocol::reExecuteQueryCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("rexecute query");

	cont->logDebugMessage("handling query...");

	cursor=cont->initQueryOrBindCursor(cursor,true,false,true);

	// get binds and whether to get column info
	if (getInputBinds(cursor) &&
		getOutputBinds(cursor) &&
		getSendColumnInfo()) {

		cont->updateState(PROCESS_SQL);
		return processQueryOrBindCursor(cursor,true,false);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cursor->getErrorNumber()) {
		returnError(cursor,true);
	}
	cont->logDebugMessage("failed to handle query");
	return false;
}

bool sqlrclientprotocol::fetchFromBindCursorCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("fetch from bind cursor");

	cursor=cont->initQueryOrBindCursor(cursor,false,true,true);

	// get whether to get column info
	if (getSendColumnInfo()) {
		cont->updateState(PROCESS_SQL);
		return processQueryOrBindCursor(cursor,false,true);
	}

	// The client is apparently sending us something we
	// can't handle.  Return an error if there was one,
	// instruct the client to disconnect and return false
	// to end the session on this side.
	if (cursor->getErrorNumber()) {
		returnError(cursor,true);
	}
	cont->logDebugMessage("failed to fetch from bind cursor");
	return false;
}

bool sqlrclientprotocol::processQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor) {

	// loop here to handle down databases
	for (;;) {

		// process the query and get skip and fetch parameters here so
		// everything can be done in one round trip without relying on
		// buffering
		if (cont->processQueryOrBindCursor(
					cursor,reexecute,bindcursor) &&
					getSkipAndFetch(cursor)) {

			cont->logDebugMessage("process query succeeded");

			// indicate that no error has occurred
			clientsock->write((uint16_t)NO_ERROR_OCCURRED);

			// send the client the id of the 
			// cursor that it's going to use
			clientsock->write(cursor->getId());

			// tell the client that this is not a
			// suspended result set
			clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

			// send a result set header
			returnResultSetHeader(cursor);

			// free memory used by binds
			bindpool->deallocate();

			// reinit total rows fetched
			cursor->clearTotalRowsFetched();

			// return the result set
			return returnResultSetData(cursor,false);

		} else {

			// if the db is still up, or if we're not configured
			// to wait for them if they're down, then return the
			// error
			if (cursor->getLiveConnection() ||
				!cont->cfgfl->getWaitForDownDatabase()) {

				// return the error
				returnError(cursor,
					!cursor->getLiveConnection());
			}

			// if the error was a dead connection
			// then re-establish the connection
			if (!cursor->getLiveConnection()) {

				cont->logDebugMessage("database is down...");

				cont->logDbError(cursor,
						cursor->getErrorBuffer());

				cont->reLogIn();

				// if we're waiting for down databases then
				// loop back and try the query again
				if (cont->cfgfl->getWaitForDownDatabase()) {
					continue;
				}
			}

			return true;
		}
	}
}

bool sqlrclientprotocol::getClientInfo(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting client info...");

	// init
	char	*clientinfo=cont->getClientInfoBuffer();
	clientinfo[0]='\0';
	uint64_t	clientinfolen=0;

	// get the length of the client info
	ssize_t	result=clientsock->read(&clientinfolen);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(cursor,
				"get client info failed: "
				"failed to get clientinfo length",result);
		cont->setClientInfoLength(0);
		return false;
	}

	// bounds checking
	if (clientinfolen>maxclientinfolength) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXCLIENTINFOLENGTH_STRING);
		err.append(" (")->append(clientinfolen)->append('>');
		err.append(maxclientinfolength)->append(')');
		cursor->setError(err.getString(),
				SQLR_ERROR_MAXCLIENTINFOLENGTH,true);

		debugstr.clear();
		debugstr.append("get client info failed: "
				"client sent bad client info size: ");
		debugstr.append(clientinfolen);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);

		cont->setClientInfoLength(0);
		return false;
	}

	// read the client info into the buffer
	result=clientsock->read(clientinfo,clientinfolen);
	if ((uint64_t)result!=clientinfolen) {
		cont->logClientProtocolError(cursor,
				"get client info failed: "
				"failed to get client info",result);
		clientinfo[0]='\0';
		cont->setClientInfoLength(0);
		return false;
	}
	clientinfo[clientinfolen]='\0';

	if (cont->loggingEnabled()) {
		debugstr.clear();
		debugstr.append("clientinfolen: ")->append(clientinfolen);
		cont->logDebugMessage(debugstr.getString());
		debugstr.clear();
		debugstr.append("clientinfo: ")->append(clientinfo);
		cont->logDebugMessage(debugstr.getString());
		cont->logDebugMessage("getting clientinfo succeeded");
	}

	// update the stats with the client info
	cont->updateClientInfo(clientinfo,clientinfolen);

	return true;
}

bool sqlrclientprotocol::getQuery(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting query...");

	// init
	uint32_t	querylength=0;
	char		*querybuffer=cursor->getQueryBuffer();
	querybuffer[0]='\0';

	// get the length of the query
	ssize_t	result=clientsock->read(&querylength,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cursor->setQueryLength(0);
		cont->logClientProtocolError(cursor,
				"get query failed: "
				"failed to get query length",result);
		return false;
	}

	// bounds checking
	if (querylength>maxquerysize) {
		cursor->setQueryLength(0);

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXQUERYLENGTH_STRING);
		err.append(" (")->append(querylength)->append('>');
		err.append(maxquerysize)->append(')');
		cursor->setError(err.getString(),
				SQLR_ERROR_MAXQUERYLENGTH,true);

		debugstr.clear();
		debugstr.append("get query failed: "
				"client sent bad query length: ");
		debugstr.append(querylength);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);

		return false;
	}

	// read the query into the buffer
	result=clientsock->read(querybuffer,querylength,idleclienttimeout,0);
	if ((uint32_t)result!=querylength) {

		cursor->setQueryLength(0);
		querybuffer[0]='\0';

		cont->logClientProtocolError(cursor,
				"get query failed: "
				"failed to get query",result);
		return false;
	}

	// update query buffer and length
	querybuffer[querylength]='\0';
	cursor->setQueryLength(querylength);

	if (cont->loggingEnabled()) {
		debugstr.clear();
		debugstr.append("querylength: ")->append(querylength);
		cont->logDebugMessage(debugstr.getString());
		debugstr.clear();
		debugstr.append("query: ")->append(querybuffer);
		cont->logDebugMessage(debugstr.getString());
		cont->logDebugMessage("getting query succeeded");
	}

	// update the stats with the current query
	cont->updateCurrentQuery(querybuffer,querylength);

	return true;
}

bool sqlrclientprotocol::getInputBinds(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting input binds...");

	// get the number of input bind variable/values
	uint16_t	inbindcount=0;
	if (!getBindVarCount(cursor,&inbindcount)) {
		return false;
	}
	cursor->setInputBindCount(inbindcount);

	// get the input bind buffers
	bindvar_svr	*inbinds=cursor->getInputBinds();

	// fill the buffers
	for (uint16_t i=0; i<inbindcount && i<maxbindcount; i++) {

		bindvar_svr	*bv=&(inbinds[i]);

		// get the variable name and type
		if (!(getBindVarName(cursor,bv) && getBindVarType(bv))) {
			return false;
		}

		// get the value
		if (bv->type==NULL_BIND) {
			getNullBind(bv);
		} else if (bv->type==STRING_BIND) {
			if (!getStringBind(cursor,bv)) {
				return false;
			}
		} else if (bv->type==INTEGER_BIND) {
			if (!getIntegerBind(bv)) {
				return false;
			}
		} else if (bv->type==DOUBLE_BIND) {
			if (!getDoubleBind(bv)) {
				return false;
			}
		} else if (bv->type==DATE_BIND) {
			if (!getDateBind(bv)) {
				return false;
			}
		} else if (bv->type==BLOB_BIND) {
			// can't fake blob binds
			cursor->setFakeInputBindsForThisQuery(false);
			if (!getLobBind(cursor,bv)) {
				return false;
			}
		} else if (bv->type==CLOB_BIND) {
			if (!getLobBind(cursor,bv)) {
				return false;
			}
		}		  
	}

	cont->logDebugMessage("done getting input binds");
	return true;
}

bool sqlrclientprotocol::getOutputBinds(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting output binds...");

	// get the number of output bind variable/values
	uint16_t	outbindcount=0;
	if (!getBindVarCount(cursor,&outbindcount)) {
		return false;
	}
	cursor->setOutputBindCount(outbindcount);

	// get the output bind buffers
	bindvar_svr	*outbinds=cursor->getOutputBinds();

	// fill the buffers
	for (uint16_t i=0; i<outbindcount && i<maxbindcount; i++) {

		bindvar_svr	*bv=&(outbinds[i]);

		// get the variable name and type
		if (!(getBindVarName(cursor,bv) && getBindVarType(bv))) {
			return false;
		}

		// get the size of the value
		if (bv->type==STRING_BIND) {
			bv->value.stringval=NULL;
			if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
				return false;
			}
			// This must be a allocated and cleared because oracle8
			// gets angry if these aren't initialized to NULL's.
			// It's possible that just the first character needs to
			// be NULL, but for now I'm just going to go ahead and
			// use allocateAndClear.
			bv->value.stringval=
				(char *)bindpool->allocateAndClear(
							bv->valuesize+1);
			cont->logDebugMessage("STRING");
		} else if (bv->type==INTEGER_BIND) {
			cont->logDebugMessage("INTEGER");
		} else if (bv->type==DOUBLE_BIND) {
			cont->logDebugMessage("DOUBLE");
			// these don't typically get set, but they get used
			// when building debug strings, so we need to
			// initialize them
			bv->value.doubleval.precision=0;
			bv->value.doubleval.scale=0;
		} else if (bv->type==DATE_BIND) {
			cont->logDebugMessage("DATE");
			bv->value.dateval.year=0;
			bv->value.dateval.month=0;
			bv->value.dateval.day=0;
			bv->value.dateval.hour=0;
			bv->value.dateval.minute=0;
			bv->value.dateval.second=0;
			bv->value.dateval.microsecond=0;
			bv->value.dateval.tz=NULL;
			// allocate enough space to store the date/time string
			// or whatever buffer a child might need to store a
			// date 512 bytes ought to be enough
			bv->value.dateval.buffersize=512;
			bv->value.dateval.buffer=
				(char *)bindpool->allocate(
						bv->value.dateval.buffersize);
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getBindSize(cursor,bv,&maxlobbindvaluelength)) {
				return false;
			}
			if (bv->type==BLOB_BIND) {
				cont->logDebugMessage("BLOB");
			} else if (bv->type==CLOB_BIND) {
				cont->logDebugMessage("CLOB");
			}
		} else if (bv->type==CURSOR_BIND) {
			cont->logDebugMessage("CURSOR");
			sqlrcursor_svr	*curs=cont->findAvailableCursor();
			if (!curs) {
				// FIXME: set error here
				return false;
			}
			curs->setState(SQLRCURSORSTATE_BUSY);
			bv->value.cursorid=curs->getId();
		}

		// init the null indicator
		bv->isnull=conn->nonNullBindValue();
	}

	cont->logDebugMessage("done getting output binds");
	return true;
}

bool sqlrclientprotocol::getBindVarCount(sqlrcursor_svr *cursor,
						uint16_t *count) {

	// init
	*count=0;

	// get the number of input bind variable/values
	ssize_t	result=clientsock->read(count,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(cursor,
				"get binds failed: "
				"failed to get bind count",result);
		*count=0;
		return false;
	}

	// bounds checking
	if (*count>maxbindcount) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXBINDCOUNT_STRING);
		err.append(" (")->append(*count)->append('>');
		err.append(maxbindcount)->append(')');
		cursor->setError(err.getString(),SQLR_ERROR_MAXBINDCOUNT,true);

		debugstr.clear();
		debugstr.append("get binds failed: "
				"client tried to send too many binds: ");
		debugstr.append(*count);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);

		*count=0;
		return false;
	}

	return true;
}

bool sqlrclientprotocol::getBindVarName(sqlrcursor_svr *cursor,
						bindvar_svr *bv) {

	// init
	bv->variablesize=0;
	bv->variable=NULL;

	// get the variable name size
	uint16_t	bindnamesize;
	ssize_t		result=clientsock->read(&bindnamesize,
						idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(cursor,
				"get binds failed: "
				"failed to get variable name length",result);
		return false;
	}

	// bounds checking
	if (bindnamesize>maxbindnamelength) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXBINDNAMELENGTH_STRING);
		err.append(" (")->append(bindnamesize)->append('>');
		err.append(maxbindnamelength)->append(')');
		cursor->setError(err.getString(),
					SQLR_ERROR_MAXBINDNAMELENGTH,true);

		debugstr.clear();
		debugstr.append("get binds failed: bad variable name length: ");
		debugstr.append(bindnamesize);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);
		return false;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)cont->getBindMappingsPool()->
					allocate(bindnamesize+2);
	bv->variable[0]=conn->bindVariablePrefix();
	result=clientsock->read(bv->variable+1,bindnamesize,
					idleclienttimeout,0);
	if (result!=bindnamesize) {
		bv->variablesize=0;
		bv->variable[0]='\0';
		cont->logClientProtocolError(cursor,
				"get binds failed: "
				"failed to get variable name",result);
		return false;
	}
	bv->variable[bindnamesize+1]='\0';

	cont->logDebugMessage(bv->variable);

	return true;
}

bool sqlrclientprotocol::getBindVarType(bindvar_svr *bv) {

	// get the type
	ssize_t	result=clientsock->read(&bv->type,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get type",result);
		return false;
	}
	return true;
}

bool sqlrclientprotocol::getBindSize(sqlrcursor_svr *cursor,
					bindvar_svr *bv, uint32_t *maxsize) {

	// init
	bv->valuesize=0;

	// get the size of the value
	ssize_t	result=clientsock->read(&(bv->valuesize),idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		bv->valuesize=0;
		cont->logClientProtocolError(cursor,
				"get binds failed: "
				"failed to get bind value length",result);
		return false;
	}

	// bounds checking
	if (bv->valuesize>*maxsize) {
		if (maxsize==&maxstringbindvaluelength) {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXSTRINGBINDVALUELENGTH_STRING);
			err.append(" (")->append(bv->valuesize)->append('>');
			err.append(*maxsize)->append(')');
			cursor->setError(err.getString(),
				SQLR_ERROR_MAXSTRINGBINDVALUELENGTH,true);
		} else {
			stringbuffer	err;
			err.append(SQLR_ERROR_MAXLOBBINDVALUELENGTH_STRING);
			err.append(" (")->append(bv->valuesize)->append('>');
			err.append(*maxsize)->append(')');
			cursor->setError(err.getString(),
				SQLR_ERROR_MAXLOBBINDVALUELENGTH,true);
		}
		debugstr.clear();
		debugstr.append("get binds failed: bad value length: ");
		debugstr.append(bv->valuesize);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);
		return false;
	}

	return true;
}

void sqlrclientprotocol::getNullBind(bindvar_svr *bv) {

	cont->logDebugMessage("NULL");

	bv->value.stringval=(char *)bindpool->allocate(1);
	bv->value.stringval[0]='\0';
	bv->valuesize=0;
	bv->isnull=conn->nullBindValue();
}

bool sqlrclientprotocol::getStringBind(sqlrcursor_svr *cursor,
						bindvar_svr *bv) {

	cont->logDebugMessage("STRING");

	// init
	bv->value.stringval=NULL;

	// get the size of the value
	if (!getBindSize(cursor,bv,&maxstringbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->allocate(bv->valuesize+1);

	// get the bind value
	ssize_t	result=clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0);
	if ((uint32_t)result!=(uint32_t)(bv->valuesize)) {
		bv->value.stringval[0]='\0';
		const char	*info="get binds failed: "
					"failed to get bind value";
		cont->logClientProtocolError(cursor,info,result);
		return false;
	}
	bv->value.stringval[bv->valuesize]='\0';
	bv->isnull=conn->nonNullBindValue();

	cont->logDebugMessage(bv->value.stringval);

	return true;
}

bool sqlrclientprotocol::getIntegerBind(bindvar_svr *bv) {

	cont->logDebugMessage("INTEGER");

	// get the value itself
	uint64_t	value;
	ssize_t		result=clientsock->read(&value,idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get bind value",result);
		return false;
	}

	// set the value
	bv->value.integerval=(int64_t)value;

	char	*intval=charstring::parseNumber(bv->value.integerval);
	cont->logDebugMessage(intval);
	delete[] intval;

	return true;
}

bool sqlrclientprotocol::getDoubleBind(bindvar_svr *bv) {

	cont->logDebugMessage("DOUBLE");

	// get the value
	ssize_t	result=clientsock->read(&(bv->value.doubleval.value),
						idleclienttimeout,0);
	if (result!=sizeof(double)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get bind value",result);
		return false;
	}

	// get the precision
	result=clientsock->read(&(bv->value.doubleval.precision),
						idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get precision",result);
		return false;
	}

	// get the scale
	result=clientsock->read(&(bv->value.doubleval.scale),
						idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get scale",result);
		return false;
	}

	char	*doubleval=charstring::parseNumber(bv->value.doubleval.value);
	cont->logDebugMessage(doubleval);
	delete[] doubleval;

	return true;
}

bool sqlrclientprotocol::getDateBind(bindvar_svr *bv) {

	cont->logDebugMessage("DATE");

	// init
	bv->value.dateval.tz=NULL;

	uint16_t	temp;

	// get the year
	ssize_t	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get year",result);
		return false;
	}
	bv->value.dateval.year=(int16_t)temp;

	// get the month
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get month",result);
		return false;
	}
	bv->value.dateval.month=(int16_t)temp;

	// get the day
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get day",result);
		return false;
	}
	bv->value.dateval.day=(int16_t)temp;

	// get the hour
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get hour",result);
		return false;
	}
	bv->value.dateval.hour=(int16_t)temp;

	// get the minute
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get minute",result);
		return false;
	}
	bv->value.dateval.minute=(int16_t)temp;

	// get the second
	result=clientsock->read(&temp,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get second",result);
		return false;
	}
	bv->value.dateval.second=(int16_t)temp;

	// get the microsecond
	uint32_t	temp32;
	result=clientsock->read(&temp32,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get microsecond",result);
		return false;
	}
	bv->value.dateval.microsecond=(int32_t)temp32;

	// get the size of the time zone
	uint16_t	length;
	result=clientsock->read(&length,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get timezone size",result);
		return false;
	}

	// FIXME: do bounds checking here

	// allocate space to store the time zone
	bv->value.dateval.tz=(char *)bindpool->allocate(length+1);

	// get the time zone
	result=clientsock->read(bv->value.dateval.tz,length,
					idleclienttimeout,0);
	if ((uint16_t)result!=length) {
		bv->value.dateval.tz[0]='\0';
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get timezone",result);
		return false;
	}
	bv->value.dateval.tz[length]='\0';

	// allocate enough space to store the date/time string
	// 64 bytes ought to be enough
	bv->value.dateval.buffersize=64;
	bv->value.dateval.buffer=(char *)bindpool->allocate(
						bv->value.dateval.buffersize);

	stringbuffer	str;
	str.append(bv->value.dateval.year)->append("-");
	str.append(bv->value.dateval.month)->append("-");
	str.append(bv->value.dateval.day)->append(" ");
	str.append(bv->value.dateval.hour)->append(":");
	str.append(bv->value.dateval.minute)->append(":");
	str.append(bv->value.dateval.second)->append(":");
	str.append(bv->value.dateval.microsecond)->append(" ");
	str.append(bv->value.dateval.tz);
	cont->logDebugMessage(str.getString());

	return true;
}

bool sqlrclientprotocol::getLobBind(sqlrcursor_svr *cursor, bindvar_svr *bv) {

	// init
	bv->value.stringval=NULL;

	if (bv->type==BLOB_BIND) {
		cont->logDebugMessage("BLOB");
	}
	if (bv->type==CLOB_BIND) {
		cont->logDebugMessage("CLOB");
	}

	// get the size of the value
	if (!getBindSize(cursor,bv,&maxlobbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	// (the +1 is to store the NULL-terminator for CLOBS)
	bv->value.stringval=(char *)bindpool->allocate(bv->valuesize+1);

	// get the bind value
	ssize_t	result=clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0);
	if ((uint32_t)result!=(uint32_t)(bv->valuesize)) {
		bv->value.stringval[0]='\0';
		cont->logClientProtocolError(cursor,
				"get binds failed: bad value",result);
		return false;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]='\0';
	bv->isnull=conn->nonNullBindValue();

	return true;
}

bool sqlrclientprotocol::getSendColumnInfo() {

	cont->logDebugMessage("get send column info...");

	uint16_t	sendcolumninfo;
	ssize_t	result=clientsock->read(&sendcolumninfo,idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get send column info failed",result);
		return false;
	}

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		cont->logDebugMessage("send column info");
	} else {
		cont->logDebugMessage("don't send column info");
	}
	cont->logDebugMessage("done getting send column info...");

	cont->setSendColumnInfo(sendcolumninfo);

	return true;
}

bool sqlrclientprotocol::getSkipAndFetch(sqlrcursor_svr *cursor) {

	// get the number of rows to skip
	ssize_t		result=clientsock->read(&skip,idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(cursor,
				"return result set data failed: "
				"failed to get rows to skip",result);
		return false;
	}

	// get the number of rows to fetch
	result=clientsock->read(&fetch,idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(cursor,
				"return result set data failed: "
				"failed to get rows to fetch",result);
		return false;
	}
	return true;
}

void sqlrclientprotocol::returnResultSetHeader(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("returning result set header...");

	// return the row counts
	cont->logDebugMessage("returning row counts...");
	sendRowCounts(cursor->knowsRowCount(),cursor->rowCount(),
			cursor->knowsAffectedRows(),cursor->affectedRows());
	cont->logDebugMessage("done returning row counts");

	uint16_t	sendcolumninfo=cont->getSendColumnInfo();

	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write(sendcolumninfo);

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		cont->logDebugMessage("column info will be sent");
	} else {
		cont->logDebugMessage("column info will not be sent");
	}


	// return the column count
	cont->logDebugMessage("returning column counts...");
	clientsock->write(cursor->colCount());
	cont->logDebugMessage("done returning column counts");


	if (sendcolumninfo==SEND_COLUMN_INFO) {

		// return the column type format
		cont->logDebugMessage("sending column type format...");
		uint16_t	format=cursor->columnTypeFormat();
		if (format==COLUMN_TYPE_IDS) {
			cont->logDebugMessage("id's");
		} else {
			cont->logDebugMessage("names");
		}
		clientsock->write(format);
		cont->logDebugMessage("done sending column type format");

		// return the column info
		cont->logDebugMessage("returning column info...");
		returnColumnInfo(cursor,format);
		cont->logDebugMessage("done returning column info");
	}


	// return the output bind vars
	returnOutputBindValues(cursor);


	// terminate the bind vars
	clientsock->write((uint16_t)END_BIND_VARS);

	cont->logDebugMessage("done returning result set header");
}

void sqlrclientprotocol::returnColumnInfo(sqlrcursor_svr *cursor,
							uint16_t format) {

	for (uint32_t i=0; i<cursor->colCount(); i++) {

		const char	*name=cursor->getColumnName(i);
		uint16_t	namelen=cursor->getColumnNameLength(i);
		uint32_t	length=cursor->getColumnLength(i);
		uint32_t	precision=cursor->getColumnPrecision(i);
		uint32_t	scale=cursor->getColumnScale(i);
		uint16_t	nullable=cursor->getColumnIsNullable(i);
		uint16_t	primarykey=cursor->getColumnIsPrimaryKey(i);
		uint16_t	unique=cursor->getColumnIsUnique(i);
		uint16_t	partofkey=cursor->getColumnIsPartOfKey(i);
		uint16_t	unsignednumber=cursor->getColumnIsUnsigned(i);
		uint16_t	zerofill=cursor->getColumnIsZeroFilled(i);
		uint16_t	binary=cursor->getColumnIsBinary(i);
		uint16_t	autoincrement=
					cursor->getColumnIsAutoIncrement(i);

		if (format==COLUMN_TYPE_IDS) {
			sendColumnDefinition(name,namelen,
					cursor->getColumnType(i),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement);
		} else {
			sendColumnDefinitionString(name,namelen,
					cursor->getColumnTypeName(i),
					cursor->getColumnTypeNameLength(i),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement);
		}
	}
}

void sqlrclientprotocol::sendRowCounts(bool knowsactual, uint64_t actual,
					bool knowsaffected, uint64_t affected) {

	cont->logDebugMessage("sending row counts...");

	// send actual rows, if that is known
	if (knowsactual) {

		char	string[30];
		charstring::printf(string,30,	
				"actual rows: %lld",	
				(long long)actual);
		cont->logDebugMessage(string);

		clientsock->write((uint16_t)ACTUAL_ROWS);
		clientsock->write(actual);

	} else {

		cont->logDebugMessage("actual rows unknown");

		clientsock->write((uint16_t)NO_ACTUAL_ROWS);
	}

	
	// send affected rows, if that is known
	if (knowsaffected) {

		char	string[46];
		charstring::printf(string,46,
				"affected rows: %lld",
				(long long)affected);
		cont->logDebugMessage(string);

		clientsock->write((uint16_t)AFFECTED_ROWS);
		clientsock->write(affected);

	} else {

		cont->logDebugMessage("affected rows unknown");

		clientsock->write((uint16_t)NO_AFFECTED_ROWS);
	}

	cont->logDebugMessage("done sending row counts");
}

void sqlrclientprotocol::returnOutputBindValues(sqlrcursor_svr *cursor) {

	if (cont->loggingEnabled()) {
		debugstr.clear();
		debugstr.append("returning ");
		debugstr.append(cursor->getOutputBindCount());
		debugstr.append(" output bind values: ");
		cont->logDebugMessage(debugstr.getString());
	}

	// run through the output bind values, sending them back
	for (uint16_t i=0; i<cursor->getOutputBindCount(); i++) {

		bindvar_svr	*bv=&(cursor->getOutputBinds()[i]);

		if (cont->loggingEnabled()) {
			debugstr.clear();
			debugstr.append(i);
			debugstr.append(":");
		}

		if (conn->bindValueIsNull(bv->isnull)) {

			if (cont->loggingEnabled()) {
				debugstr.append("NULL");
			}

			clientsock->write((uint16_t)NULL_DATA);

		} else if (bv->type==BLOB_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("BLOB:");
			}

			returnOutputBindBlob(cursor,i);

		} else if (bv->type==CLOB_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("CLOB:");
			}

			returnOutputBindClob(cursor,i);

		} else if (bv->type==STRING_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("STRING:");
				debugstr.append(bv->value.stringval);
			}

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==INTEGER_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("INTEGER:");
				debugstr.append(bv->value.integerval);
			}

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==DOUBLE_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("DOUBLE:");
				debugstr.append(bv->value.doubleval.value);
				debugstr.append("(");
				debugstr.append(bv->value.doubleval.precision);
				debugstr.append(",");
				debugstr.append(bv->value.doubleval.scale);
				debugstr.append(")");
			}

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==DATE_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("DATE:");
				debugstr.append(bv->value.dateval.year);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.month);
				debugstr.append("-");
				debugstr.append(bv->value.dateval.day);
				debugstr.append(" ");
				debugstr.append(bv->value.dateval.hour);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.minute);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.second);
				debugstr.append(":");
				debugstr.append(bv->value.dateval.microsecond);
				debugstr.append(" ");
				debugstr.append(bv->value.dateval.tz);
			}

			clientsock->write((uint16_t)DATE_DATA);
			clientsock->write((uint16_t)bv->value.dateval.year);
			clientsock->write((uint16_t)bv->value.dateval.month);
			clientsock->write((uint16_t)bv->value.dateval.day);
			clientsock->write((uint16_t)bv->value.dateval.hour);
			clientsock->write((uint16_t)bv->value.dateval.minute);
			clientsock->write((uint16_t)bv->value.dateval.second);
			clientsock->write((uint32_t)bv->value.
							dateval.microsecond);
			uint16_t	length=charstring::length(
							bv->value.dateval.tz);
			clientsock->write(length);
			clientsock->write(bv->value.dateval.tz,length);

		} else if (bv->type==CURSOR_BIND) {

			if (cont->loggingEnabled()) {
				debugstr.append("CURSOR:");
				debugstr.append(bv->value.cursorid);
			}

			clientsock->write((uint16_t)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		if (cont->loggingEnabled()) {
			cont->logDebugMessage(debugstr.getString());
		}
	}

	cont->logDebugMessage("done returning output bind values");
}

void sqlrclientprotocol::returnOutputBindBlob(sqlrcursor_svr *cursor,
							uint16_t index) {
	sendLobOutputBind(cursor,index);
	cursor->cleanUpLobOutputBind(index);
}

void sqlrclientprotocol::returnOutputBindClob(sqlrcursor_svr *cursor,
							uint16_t index) {
	sendLobOutputBind(cursor,index);
	cursor->cleanUpLobOutputBind(index);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrclientprotocol::sendLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cursor->getLobOutputBindLength(index,&loblength)) {
		sendNullField();
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		startSendingLong(0);
		sendLongSegment("",0);
		endSendingLong();
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cursor->getLobOutputBindSegment(
					index,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				sendNullField();
			} else {
				endSendingLong();
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			sendLongSegment(lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrclientprotocol::sendColumnDefinition(const char *name,
						uint16_t namelen,
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement) {

	if (cont->loggingEnabled()) {
		debugstr.clear();
		for (uint16_t i=0; i<namelen; i++) {
			debugstr.append(name[i]);
		}
		debugstr.append(":");
		debugstr.append(type);
		debugstr.append(":");
		debugstr.append(size);
		debugstr.append(" (");
		debugstr.append(precision);
		debugstr.append(",");
		debugstr.append(scale);
		debugstr.append(") ");
		if (!nullable) {
			debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			debugstr.append("Primary key ");
		}
		if (unique) {
			debugstr.append("Unique");
		}
		cont->logDebugMessage(debugstr.getString());
	}

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(type);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);
}

void sqlrclientprotocol::sendColumnDefinitionString(const char *name,
						uint16_t namelen,
						const char *type, 
						uint16_t typelen,
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement) {

	if (cont->loggingEnabled()) {
		debugstr.clear();
		for (uint16_t ni=0; ni<namelen; ni++) {
			debugstr.append(name[ni]);
		}
		debugstr.append(":");
		for (uint16_t ti=0; ti<typelen; ti++) {
			debugstr.append(type[ti]);
		}
		debugstr.append(":");
		debugstr.append(size);
		debugstr.append(" (");
		debugstr.append(precision);
		debugstr.append(",");
		debugstr.append(scale);
		debugstr.append(") ");
		if (!nullable) {
			debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			debugstr.append("Primary key ");
		}
		if (unique) {
			debugstr.append("Unique");
		}
		cont->logDebugMessage(debugstr.getString());
	}

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(typelen);
	clientsock->write(type,typelen);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);
}

bool sqlrclientprotocol::returnResultSetData(sqlrcursor_svr *cursor,
						bool getskipandfetch) {

	cont->logDebugMessage("returning result set data...");

	cont->updateState(RETURN_RESULT_SET);

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->getCustomQueryCursor()) {
		cursor=cursor->getCustomQueryCursor();
	}

	// get the number of rows to skip and fetch
	if (getskipandfetch) {
		if (!getSkipAndFetch(cursor)) {
			return false;
		}
	}

	// reinit cursor state (in case it was suspended)
	cursor->setState(SQLRCURSORSTATE_BUSY);

	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((uint16_t)END_RESULT_SET);
		clientsock->flushWriteBuffer(-1,-1);
		cont->logDebugMessage("done returning result set data");
		return true;
	}

	// skip the specified number of rows
	if (!cont->skipRows(cursor,skip)) {
		clientsock->write((uint16_t)END_RESULT_SET);
		clientsock->flushWriteBuffer(-1,-1);
		cont->logDebugMessage("done returning result set data");
		return true;
	}


	if (cont->loggingEnabled()) {
		debugstr.clear();
		debugstr.append("fetching ");
		debugstr.append(fetch);
		debugstr.append(" rows...");
		cont->logDebugMessage(debugstr.getString());
	}

	// send the specified number of rows back
	for (uint64_t i=0; (!fetch || i<fetch); i++) {

		if (!cursor->fetchRow()) {
			clientsock->write((uint16_t)END_RESULT_SET);
			break;
		}

		cursor->incrementTotalRowsFetched();

		if (cont->loggingEnabled()) {
			debugstr.clear();
		}

		returnRow(cursor);

		if (cont->loggingEnabled()) {
			cont->logDebugMessage(debugstr.getString());
		}
	}
	clientsock->flushWriteBuffer(-1,-1);

	cont->logDebugMessage("done returning result set data");
	return true;
}

void sqlrclientprotocol::returnRow(sqlrcursor_svr *cursor) {

	// run through the columns...
	for (uint32_t i=0; i<cursor->colCount(); i++) {

		// init variables
		const char	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;

		// get the field
		cursor->getField(i,&field,&fieldlength,&blob,&null);

		// send data to the client
		if (null) {
			sendNullField();
		} else if (blob) {
			sendLobField(cursor,i);
			cursor->cleanUpLobField(i);
		} else {
			const char	*newfield=NULL;
			uint32_t	newfieldlength=0;
			cont->reformatField(cursor,i,field,fieldlength,
						&newfield,&newfieldlength);
			sendField(newfield,newfieldlength);
		}
	}

	// get the next row
	cursor->nextRow();
}

void sqlrclientprotocol::sendField(const char *data, uint32_t size) {

	if (cont->loggingEnabled()) {
		debugstr.append("\"");
		debugstr.append(data,size);
		debugstr.append("\",");
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrclientprotocol::sendNullField() {

	if (cont->loggingEnabled()) {
		debugstr.append("NULL");
	}
	clientsock->write((uint16_t)NULL_DATA);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrclientprotocol::sendLobField(sqlrcursor_svr *cursor, uint32_t col) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cursor->getLobFieldLength(col,&loblength)) {
		sendNullField();
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		startSendingLong(0);
		sendLongSegment("",0);
		endSendingLong();
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cursor->getLobFieldSegment(col,
					lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				sendNullField();
			} else {
				endSendingLong();
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			sendLongSegment(lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrclientprotocol::startSendingLong(uint64_t longlength) {
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrclientprotocol::sendLongSegment(const char *data, uint32_t size) {

	if (cont->loggingEnabled()) {
		debugstr.append(data,size);
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrclientprotocol::endSendingLong() {

	if (cont->loggingEnabled()) {
		debugstr.append(",");
	}

	clientsock->write((uint16_t)END_LONG_DATA);
}

void sqlrclientprotocol::returnError(bool disconnect) {

	// Get the error data if none is set already
	// FIXME: this will always evaluate false,
	// shouldn't it be !conn->getErrorLength()?
	if (!conn->getErrorBuffer()) {
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		conn->errorMessage(conn->getErrorBuffer(),
					maxerrorlength,
					&errorlength,&errnum,&liveconnection);
		conn->setErrorLength(errorlength);
		conn->setErrorNumber(errnum);
		conn->setLiveConnection(liveconnection);
		if (!liveconnection) {
			disconnect=true;
		}
	}

	// send the appropriate error status
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code and error string
	clientsock->write((uint64_t)conn->getErrorNumber());

	// send the error string
	clientsock->write((uint16_t)conn->getErrorLength());
	clientsock->write(conn->getErrorBuffer(),conn->getErrorLength());
	clientsock->flushWriteBuffer(-1,-1);

	cont->logDbError(NULL,conn->getErrorBuffer());
}

void sqlrclientprotocol::returnError(sqlrcursor_svr *cursor, bool disconnect) {

	cont->logDebugMessage("returning error...");

	// send the appropriate error status
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)cursor->getErrorNumber());

	// send the error string
	clientsock->write((uint16_t)cursor->getErrorLength());
	clientsock->write(cursor->getErrorBuffer(),cursor->getErrorLength());

	// client will be sending skip/fetch, better get
	// it even though we're not going to use it
	uint64_t	skipfetch;
	clientsock->read(&skipfetch,idleclienttimeout,0);
	clientsock->read(&skipfetch,idleclienttimeout,0);

	// Even though there was an error, we still 
	// need to send the client the id of the 
	// cursor that it's going to use.
	clientsock->write(cursor->getId());
	clientsock->flushWriteBuffer(-1,-1);

	cont->logDebugMessage("done returning error");

	cont->logDbError(cursor,cursor->getErrorBuffer());
}

bool sqlrclientprotocol::fetchResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("fetching result set...");
	bool	retval=returnResultSetData(cursor,true);
	cont->logDebugMessage("done fetching result set");
	return retval;
}

void sqlrclientprotocol::abortResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("aborting result set...");
	cursor->abort();
	cont->logDebugMessage("done aborting result set");
}

void sqlrclientprotocol::suspendResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("suspend result set...");
	cursor->setState(SQLRCURSORSTATE_SUSPENDED);
	if (cursor->getCustomQueryCursor()) {
		cursor->getCustomQueryCursor()->
			setState(SQLRCURSORSTATE_SUSPENDED);
	}
	cont->logDebugMessage("done suspending result set");
}

bool sqlrclientprotocol::resumeResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("resume result set...");

	bool	retval=true;

	if (cursor->getState()==SQLRCURSORSTATE_SUSPENDED) {

		cont->logDebugMessage("previous result set was suspended");

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cursor->getId());
		clientsock->write((uint16_t)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the index of the last row that
		// was fetched to the client then resume the result set
		// (FIXME: 0 will be sent if no rows were fetched or if
		// only the first row was fetched. This probably isn't
		// correct.)
		uint64_t	totalrowsfetched=cursor->getTotalRowsFetched();
		clientsock->write((totalrowsfetched)?totalrowsfetched-1:0);

		returnResultSetHeader(cursor);
		retval=returnResultSetData(cursor,true);

	} else {

		cont->logDebugMessage("previous result set was not suspended");

		// indicate that an error has occurred
		clientsock->write((uint16_t)ERROR_OCCURRED);

		// send the error code (zero for now)
		clientsock->write((uint64_t)SQLR_ERROR_RESULTSETNOTSUSPENDED);

		// send the error itself
		uint16_t	len=charstring::length(
				SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING);
		clientsock->write(len);
		clientsock->write(SQLR_ERROR_RESULTSETNOTSUSPENDED_STRING,len);

		retval=false;
	}

	cont->logDebugMessage("done resuming result set");
	return retval;
}

bool sqlrclientprotocol::getDatabaseListCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("get db list...");
	bool	retval=getListCommand(cursor,0,false);
	cont->logDebugMessage("done getting db list");
	return retval;
}

bool sqlrclientprotocol::getTableListCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("get table list...");
	bool	retval=getListCommand(cursor,1,false);
	cont->logDebugMessage("done getting table list");
	return retval;
}

bool sqlrclientprotocol::getColumnListCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("get column list...");
	bool	retval=getListCommand(cursor,2,true);
	cont->logDebugMessage("done getting column list");
	return retval;
}


bool sqlrclientprotocol::getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable) {

	// clean up any custom query cursors
	if (cursor->getCustomQueryCursor()) {
		cursor->getCustomQueryCursor()->close();
		cursor->clearCustomQueryCursor();
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData();

	// get length of wild parameter
	uint32_t	wildlen;
	ssize_t		result=clientsock->read(&wildlen,idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(cursor,
				"get list failed: "
				"failed to get wild length",result);
		return false;
	}

	// bounds checking
	if (wildlen>maxquerysize) {
		debugstr.clear();
		debugstr.append("get list failed: wild length too large: ");
		debugstr.append(wildlen);
		cont->logClientProtocolError(cursor,debugstr.getString(),1);
		return false;
	}

	// read the wild parameter into the buffer
	char	*wild=new char[wildlen+1];
	if (wildlen) {
		result=clientsock->read(wild,wildlen,idleclienttimeout,0);
		if ((uint32_t)result!=wildlen) {
			cont->logClientProtocolError(cursor,
					"get list failed: "
					"failed to get wild parameter",result);
			return false;
		}
	}
	wild[wildlen]='\0';

	// read the table parameter into the buffer
	char	*table=NULL;
	if (gettable) {

		// get length of table parameter
		uint32_t	tablelen;
		result=clientsock->read(&tablelen,idleclienttimeout,0);
		if (result!=sizeof(uint32_t)) {
			cont->logClientProtocolError(cursor,
					"get list failed: "
					"failed to get table length",result);
			return false;
		}

		// bounds checking
		if (tablelen>maxquerysize) {
			debugstr.clear();
			debugstr.append("get list failed: "
					"table length too large: ");
			debugstr.append(tablelen);
			cont->logClientProtocolError(
					cursor,debugstr.getString(),1);
			return false;
		}

		// read the table parameter into the buffer
		table=new char[tablelen+1];
		if (tablelen) {
			result=clientsock->read(table,tablelen,
						idleclienttimeout,0);
			if ((uint32_t)result!=tablelen) {
				cont->logClientProtocolError(cursor,
					"get list failed: "
					"failed to get table parameter",result);
				return false;
			}
		}
		table[tablelen]='\0';

		// some apps aren't well behaved, trim spaces off of both sides
		charstring::bothTrim(table);

		// translate table name, if necessary
		const char	*newtable=cont->translateTableName(table);
		if (newtable) {
			delete[] table;
			table=charstring::duplicate(newtable);
		}
	}

	// set the values that we won't get from the client
	cursor->setInputBindCount(0);
	cursor->setOutputBindCount(0);
	cont->setSendColumnInfo(SEND_COLUMN_INFO);

	// get the list and return it
	bool	retval=true;
	if (conn->getListsByApiCalls()) {
		retval=getListByApiCall(cursor,which,table,wild);
	} else {
		retval=getListByQuery(cursor,which,table,wild);
	}

	// clean up
	delete[] wild;
	delete[] table;

	return retval;
}

bool sqlrclientprotocol::getListByApiCall(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild) {

	// initialize flags andbuffers
	bool	success=false;

	// get the appropriate list
	switch (which) {
		case 0:
			success=conn->getDatabaseList(cursor,wild);
			break;
		case 1:
			success=conn->getTableList(cursor,wild);
			break;
		case 2:
			success=conn->getColumnList(cursor,table,wild);
			break;
	}

	if (success) {
		success=getSkipAndFetch(cursor);
	}

	// if an error occurred...
	if (!success) {
		uint32_t	errorlength;
		int64_t		errnum;
		bool		liveconnection;
		cursor->errorMessage(cursor->getErrorBuffer(),
					maxerrorlength,
					&errorlength,&errnum,&liveconnection);
		cursor->setErrorLength(errorlength);
		cursor->setErrorNumber(errnum);
		cursor->setLiveConnection(liveconnection);
		returnError(cursor,!liveconnection);

		// this is actually OK, only return false on a network error
		return true;
	}

	// indicate that no error has occurred
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);

	// send the client the id of the 
	// cursor that it's going to use
	clientsock->write(cursor->getId());

	// tell the client that this is not a
	// suspended result set
	clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

	// if the query processed ok then send a result set header and return...
	returnResultSetHeader(cursor);
	if (!returnResultSetData(cursor,false)) {
		return false;
	}
	return true;
}

bool sqlrclientprotocol::getListByQuery(sqlrcursor_svr *cursor,
						int which,
						const char *table,
						const char *wild) {

	// build the appropriate query
	const char	*query=NULL;
	bool		havewild=charstring::length(wild);
	switch (which) {
		case 0:
			query=conn->getDatabaseListQuery(havewild);
			break;
		case 1:
			query=conn->getTableListQuery(havewild);
			break;
		case 2:
			query=conn->getColumnListQuery(table,havewild);
			break;
	}

	// FIXME: this can fail
	buildListQuery(cursor,query,wild,table);

	cont->logDebugMessage("handling query...");

	cursor=cont->initQueryOrBindCursor(cursor,false,false,false);

	cont->updateState(PROCESS_SQL);

	return processQueryOrBindCursor(cursor,false,false);
}

bool sqlrclientprotocol::buildListQuery(sqlrcursor_svr *cursor,
						const char *query,
						const char *wild,
						const char *table) {

	// clean up buffers to avoid SQL injection
	stringbuffer	wildbuf;
	escapeParameter(&wildbuf,wild);
	stringbuffer	tablebuf;
	escapeParameter(&tablebuf,table);

	// bounds checking
	cursor->setQueryLength(charstring::length(query)+
					wildbuf.getStringLength()+
					tablebuf.getStringLength());
	if (cursor->getQueryLength()>maxquerysize) {
		return false;
	}

	// fill the query buffer and update the length
	char	*querybuffer=cursor->getQueryBuffer();
	if (tablebuf.getStringLength()) {
		charstring::printf(querybuffer,maxquerysize+1,
						query,tablebuf.getString(),
						wildbuf.getString());
	} else {
		charstring::printf(querybuffer,maxquerysize+1,
						query,wildbuf.getString());
	}
	cursor->setQueryLength(charstring::length(querybuffer));
	return true;
}

void sqlrclientprotocol::escapeParameter(stringbuffer *buffer,
						const char *parameter) {

	if (!parameter) {
		return;
	}

	// escape single quotes
	for (const char *ptr=parameter; *ptr; ptr++) {
		if (*ptr=='\'') {
			buffer->append('\'');
		}
		buffer->append(*ptr);
	}
}

bool sqlrclientprotocol::getQueryTreeCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting query tree");

	// get the tree as a string
	xmldom		*tree=cursor->getQueryTree();
	xmldomnode	*root=(tree)?tree->getRootNode():NULL;
	stringbuffer	*xml=(root)?root->xml():NULL;
	const char	*xmlstring=(xml)?xml->getString():NULL;
	uint64_t	xmlstringlen=(xml)?xml->getStringLength():0;

	// send the tree
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write(xmlstringlen);
	clientsock->write(xmlstring,xmlstringlen);
	clientsock->flushWriteBuffer(-1,-1);

	// clean up
	delete xml;

	return true;
}

void sqlrclientprotocol::closeClientSession() {

	cont->closeClientSocket(
				// sending auth
				(sizeof(uint16_t)+
				// user/password
				2*(sizeof(uint32_t)+USERSIZE)+
				// sending query
				sizeof(uint16_t)+
				// need a cursor
				sizeof(uint16_t)+
				// executing new query
				sizeof(uint16_t)+
				// query size and query
				sizeof(uint32_t)+maxquerysize+
				// input bind var count
				sizeof(uint16_t)+
				// input bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				maxbindcount*(2*sizeof(uint16_t)+
						maxbindnamelength)+
				// get column info
				sizeof(uint16_t)+
				// skip/fetch
				2*sizeof(uint32_t)
				// divide by two because we're
				// reading 2 bytes at a time
				)/2);
}
