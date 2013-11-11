// Copyright (c) 1999-2013  David Muse
// See the file COPYING for more information

#include <sqlrclientprotocol.h>

sqlrclientprotocol::sqlrclientprotocol(sqlrcontroller_svr *cont,
					sqlrconnection_svr *conn,
					filedescriptor *clientsock) :
					sqlrprotocol(cont,conn,clientsock) {
}

sqlrclientprotocol::~sqlrclientprotocol() {
}

void sqlrclientprotocol::clientSession() {

	// During each session, the client will send a series of commands.
	// The session ends when the client ends it or when certain commands
	// fail.
	bool		loop=true;
	bool		endsession=true;
	uint16_t	command;
	do {

		// get a command from the client
		if (!getCommand(&command)) {
			break;
		}

		// get the command start time
		timeval		tv;
		gettimeofday(&tv,NULL);

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
			endsession=false;
			break;
		} else if (command==END_SESSION) {
			cont->incrementEndSessionCount();
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
		cursor->commandstartsec=tv.tv_sec;
		cursor->commandstartusec=tv.tv_usec;

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
		gettimeofday(&tv,NULL);
		cursor->commandendsec=tv.tv_sec;
		cursor->commandendusec=tv.tv_usec;

		// log query-related commands
		// FIXME: this won't log triggers
		if (command==NEW_QUERY ||
			command==REEXECUTE_QUERY ||
			command==FETCH_FROM_BIND_CURSOR) {
			cont->logQuery(cursor);
		}

	} while (loop);

	if (endsession) {
		cont->endSession();
	}

	cont->closeClientSocket();
	cont->closeSuspendedSessionSockets();

	const char	*info="an error occurred";
	if (command==NO_COMMAND) {
		info="client closed connection";
	} else if (command==END_SESSION) {
		info="client ended the session";
	} else if (command==SUSPEND_SESSION) {
		info="client suspended the session";
	}
	cont->logClientDisconnected(info);

	cont->decrementOpenClientConnections();
	inclientsession=false;

	cont->logDebugMessage("done with client session");
}

bool sqlrcontroller_svr::getCommand(uint16_t *command) {

	cont->logDebugMessage("getting command...");

	cont->updateState(GET_COMMAND);

	// get the command
	ssize_t	result=clientsock->read(command,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {

		// Return false but don't consider it an error if we get a
		// timeout or a 0 (meaning that the client closed the socket)
		// as either would be natural to do here.
		if (result!=RESULT_TIMEOUT && result!=0) {
			cont->logClientProtocolError(NULL,
				"get command failed",result);
		}

		*command=NO_COMMAND;
		return false;
	}

	cont->debugstr.clear();
	cont->debugstr.append("command: ")->append(*command);
	cont->logDebugMessage(cont->debugstr.getString());

	cont->logDebugMessage("done getting command");
	return true;
}

sqlrcursor_svr *sqlrcontroller_svr::getCursor(uint16_t command) {

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
						cont->idleclienttimeout,0);
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
						cont->idleclienttimeout,0);
		if (result!=sizeof(uint16_t)) {
			cont->logClientProtocolError(NULL,
					"get cursor failed: "
					"failed to get cursor id",result);
			return NULL;
		}

		// get the current cursor that they requested.
		bool	found=false;
		for (uint16_t i=0; i<cont->cursorcount; i++) {
			if (cont->cur[i]->id==id) {
				cursor=cont->cur[i];
				cont->incrementTimesCursorReused();
				found=true;
				break;
			}
		}

		// don't allow the client to request a cursor 
		// beyond the end of the array.
		if (!found) {
			cont->debugstr.clear();
			cont->debugstr.append("get cursor failed: "
					"client requested an invalid cursor: ");
			cont->debugstr.append(id);
			cont->logClientProtocolError(NULL,cont->debugstr.getString(),1);
			return NULL;
		}

	} else {

		// find an available cursor
		cursor=findAvailableCursor();

 		// mark this as a new cursor being used
		if (cursor) {
			cont->incrementTimesNewCursorUsed();
		}
	}

	cont->logDebugMessage("done getting a cursor");
	return cursor;
}

void sqlrcontroller_svr::noAvailableCursors(uint16_t command) {

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
	clientsock->read(dummy,size,cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::authenticateCommand() {

	cont->logDebugMessage("authenticate");

	// get the user/password from the client and authenticate
	if (getUserFromClient() && getPasswordFromClient() && cont->authenticate()) {
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

bool sqlrcontroller_svr::getUserFromClient() {
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get user size",result);
		return false;
	}
	if (size>=sizeof(cont->userbuffer)) {
		cont->debugstr.clear();
		cont->debugstr.append("authentication failed: user size too long: ");
		cont->debugstr.append(size);
		cont->logClientConnectionRefused(cont->debugstr.getString());
		return false;
	}
	result=clientsock->read(cont->userbuffer,size,cont->idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get user",result);
		return false;
	}
	cont->userbuffer[size]='\0';
	return true;
}

bool sqlrcontroller_svr::getPasswordFromClient() {
	uint32_t	size=0;
	ssize_t		result=clientsock->read(&size,cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get password size",result);
		return false;
	}
	if (size>=sizeof(cont->passwordbuffer)) {
		cont->debugstr.clear();
		cont->debugstr.append("authentication failed: "
				"password size too long: ");
		cont->debugstr.append(size);
		cont->logClientConnectionRefused(cont->debugstr.getString());
		return false;
	}
	result=clientsock->read(cont->passwordbuffer,size,cont->idleclienttimeout,0);
	if ((uint32_t)result!=size) {
		cont->logClientProtocolError(NULL,
			"authentication failed: "
			"failed to get password",result);
		return false;
	}
	cont->passwordbuffer[size]='\0';
	return true;
}

void sqlrcontroller_svr::suspendSessionCommand() {

	cont->logDebugMessage("suspending session...");

	// mark the session suspended
	cont->suspendedsession=true;

	// we can't wait forever for the client to resume, set a timeout
	cont->accepttimeout=cfgfl->getSessionTimeout();

	// abort all cursors that aren't suspended...
	cont->logDebugMessage("aborting busy cursors...");
	for (int32_t i=0; i<cont->cursorcount; i++) {
		if (cont->cur[i]->state==SQLRCURSORSTATE_BUSY) {
			cont->cur[i]->abort();
		}
	}
	cont->logDebugMessage("done aborting busy cursors");

	// open sockets to resume on
	cont->logDebugMessage("opening sockets to resume on...");
	uint16_t	unixsocketsize=0;
	uint16_t	inetportnumber=0;
	if (cong->openSockets()) {
		if (cont->serversockun) {
			unixsocketsize=charstring::length(cont->unixsocket);
		}
		inetportnumber=inetport;
	}
	cont->logDebugMessage("done opening sockets to resume on");

	// pass the socket info to the client
	cont->logDebugMessage("passing socket info to client...");
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	clientsock->write(unixsocketsize);
	if (unixsocketsize) {
		clientsock->write(cont->unixsocket,unixsocketsize);
	}
	clientsock->write(inetportnumber);
	clientsock->flushWriteBuffer(-1,-1);
	cont->logDebugMessage("done passing socket info to client");

	cont->logDebugMessage("done suspending session");
}

void sqlrcontroller_svr::pingCommand() {
	cont->logDebugMessage("ping");
	bool	pingresult=conn->ping();
	if (pingresult) {
		cont->logDebugMessage("ping succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("ping failed");
		returnError(!conn->liveconnection);
	}
	if (!pingresult) {
		cont->reLogIn();
	}
}

void sqlrcontroller_svr::identifyCommand() {

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

void sqlrcontroller_svr::autoCommitCommand() {
	cont->logDebugMessage("autocommit...");
	bool	on;
	ssize_t	result=clientsock->read(&on,cont->idleclienttimeout,0);
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
		returnError(!conn->liveconnection);
	}
}

void sqlrcontroller_svr::beginCommand() {
	cont->logDebugMessage("begin...");
	if (cont->begin()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->liveconnection);
	}
}

void sqlrcontroller_svr::commitCommand() {
	cont->logDebugMessage("commit...");
	if (cont->commit()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->liveconnection);
	}
}

void sqlrcontroller_svr::rollbackCommand() {
	cont->logDebugMessage("rollback...");
	if (cont->rollback()) {
		cont->logDebugMessage("succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("failed");
		returnError(!conn->liveconnection);
	}
}

void sqlrcontroller_svr::dbVersionCommand() {

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

void sqlrcontroller_svr::bindFormatCommand() {

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

void sqlrcontroller_svr::serverVersionCommand() {

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

void sqlrcontroller_svr::selectDatabaseCommand() {

	cont->logDebugMessage("select database");

	// get length of db parameter
	uint32_t	dblen;
	ssize_t		result=clientsock->read(&dblen,cont->idleclienttimeout,0);
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
		cont->debugstr.clear();
		cont->debugstr.append("select database failed: "
				"client sent bad db length: ");
		cont->debugstr.append(dblen);
		cont->logClientProtocolError(NULL,cont->debugstr.getString(),1);
		return;
	}

	// read the db parameter into the buffer
	char	*db=new char[dblen+1];
	if (dblen) {
		result=clientsock->read(db,dblen,cont->idleclienttimeout,0);
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
	
	// Select the db and send back the result.  If we've been told to
	// ignore these calls, skip the actual call but act like it succeeded.
	if ((ignoreselectdb)?true:conn->selectDatabase(db)) {
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		returnError(!conn->liveconnection);
	}

	delete[] db;

	return;
}

void sqlrcontroller_svr::getCurrentDatabaseCommand() {

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

void sqlrcontroller_svr::getLastInsertIdCommand() {
	cont->logDebugMessage("getting last insert id...");
	uint64_t	id;
	if (conn->getLastInsertId(&id)) {
		cont->logDebugMessage("get last insert id succeeded");
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);
		clientsock->write(id);
		clientsock->flushWriteBuffer(-1,-1);
	} else {
		cont->logDebugMessage("get last insert id failed");
		returnError(!conn->liveconnection);
	}
}

void sqlrcontroller_svr::dbHostNameCommand() {

	cont->logDebugMessage("getting db host name");

	const char	*hostname=conn->dbHostName();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	hostnamelen=charstring::length(hostname);
	clientsock->write(hostnamelen);
	clientsock->write(hostname,hostnamelen);
	clientsock->flushWriteBuffer(-1,-1);
}

void sqlrcontroller_svr::dbIpAddressCommand() {

	cont->logDebugMessage("getting db host name");

	const char	*ipaddress=conn->dbIpAddress();
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);
	uint16_t	ipaddresslen=charstring::length(ipaddress);
	clientsock->write(ipaddresslen);
	clientsock->write(ipaddress,ipaddresslen);
	clientsock->flushWriteBuffer(-1,-1);
}

bool sqlrcontroller_svr::newQueryCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("new query");
	return handleQueryOrBindCursor(cursor,false,false,true);
}

bool sqlrcontroller_svr::reExecuteQueryCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("rexecute query");
	return handleQueryOrBindCursor(cursor,true,false,true);
}

bool sqlrcontroller_svr::fetchFromBindCursorCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("fetch from bind cursor");
	return handleQueryOrBindCursor(cursor,false,true,true);
}

bool sqlrcontroller_svr::handleQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor,
							bool getquery) {


	cont->logDebugMessage("handling query...");

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->customquerycursor) {
		if (reexecute) {
			cursor=cursor->customquerycursor;
		} else {
			cursor->customquerycursor->close();
			delete cursor->customquerycursor;
			cursor->customquerycursor=NULL;
		}
	}

	// re-init error data
	cursor->clearError();

	// clear bind mappings and reset the fake input binds flag
	// (do this here because getInput/OutputBinds uses the bindmappingspool)
	if (!bindcursor && !reexecute) {
		bindmappingspool->deallocate();
		inbindmappings->clear();
		outbindmappings->clear();
		cursor->fakeinputbindsforthisquery=fakeinputbinds;
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData();

	// get the query and bind data from the client...
	bool	usingcustomquerycursor=false;
	if (getquery) {

		// re-init buffers
		if (!reexecute && !bindcursor) {
			clientinfo[0]='\0';
			clientinfolen=0;
		}
		if (!reexecute) {
			cursor->querybuffer[0]='\0';
			cursor->querylength=0;
		}
		cursor->inbindcount=0;
		cursor->outbindcount=0;
		for (uint16_t i=0; i<maxbindcount; i++) {
			rawbuffer::zero(&(cursor->inbindvars[i]),
						sizeof(bindvar_svr));
			rawbuffer::zero(&(cursor->outbindvars[i]),
						sizeof(bindvar_svr));
		}

		// get the data
		bool	success=true;
		if (!reexecute && !bindcursor) {
			success=(getClientInfo(cursor) &&
					getQuery(cursor));

			// do we need to use a custom query
			// handler for this query?
			if (success && sqlrq) {
				cursor->customquerycursor=
					sqlrq->match(conn,
						cursor->querybuffer,
						cursor->querylength);
				
			}

			if (cursor->customquerycursor) {

				// open the cursor
				cursor->customquerycursor->openInternal(
								cursor->id);

				// copy the query that we just got into the
				// custom query cursor
				charstring::copy(
					cursor->customquerycursor->querybuffer,
					cursor->querybuffer);
				cursor->customquerycursor->querylength=
							cursor->querylength;

				// set the cursor state
				cursor->customquerycursor->state=
						SQLRCURSORSTATE_BUSY;

				// reset the rest of this method to use
				// the custom query cursor
				cursor=cursor->customquerycursor;

				usingcustomquerycursor=true;
			}
		}
		if (success && !bindcursor) {
			success=(getInputBinds(cursor) &&
					getOutputBinds(cursor));
		}
		if (success) {
			success=getSendColumnInfo();
		}
		if (!success) {
			// The client is apparently sending us something we
			// can't handle.  Return an error if there was one,
			// instruct the client to disconnect and return false
			// to end the session on this side.
			if (cursor->errnum) {
				returnError(cursor,true);
			}
			cont->logDebugMessage("failed to handle query");
			return false;
		}
	}

	cont->updateState((usingcustomquerycursor)?PROCESS_CUSTOM:PROCESS_SQL);

	// loop here to handle down databases
	for (;;) {

		// process the query
		bool	success=false;
		bool	wasfaketransactionquery=false;
		if (!reexecute && !bindcursor && faketransactionblocks) {
			success=handleFakeTransactionQueries(cursor,
						&wasfaketransactionquery);
		}
		if (!wasfaketransactionquery) {
			success=processQuery(cursor,reexecute,bindcursor);
		}

		if (success) {
			// get skip and fetch parameters here so everything
			// can be done in one round trip without relying on
			// buffering
			success=getSkipAndFetch(cursor);
		}

		if (success) {

			// indicate that no error has occurred
			clientsock->write((uint16_t)NO_ERROR_OCCURRED);

			// send the client the id of the 
			// cursor that it's going to use
			clientsock->write(cursor->id);

			// tell the client that this is not a
			// suspended result set
			clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

			// if the query processed 
			// ok then send a result set
			// header and return...
			returnResultSetHeader(cursor);

			// free memory used by binds
			bindpool->deallocate();

			cont->logDebugMessage("handle query succeeded");

			// reinit lastrow
			cursor->lastrowvalid=false;

			// return the result set
			//return returnResultSetData(cursor,false);
			bool	retval=returnResultSetData(cursor,false);
			return retval;

		} else {

			// if the db is still up, or if we're not waiting
			// for them if they're down, then return the error
			if (cursor->liveconnection ||
				!cfgfl->getWaitForDownDatabase()) {

				// return the error
				returnError(cursor,!cursor->liveconnection);
			}

			// if the error was a dead connection
			// then re-establish the connection
			if (!cursor->liveconnection) {

				cont->logDebugMessage("database is down...");

				logDbError(cursor,cursor->error);

				cont->reLogIn();

				// if we're waiting for down databases then
				// loop back and try the query again
				if (cfgfl->getWaitForDownDatabase()) {
					continue;
				}
			}

			return true;
		}
	}
}

bool sqlrcontroller_svr::getClientInfo(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting client info...");

	// init
	clientinfolen=0;
	clientinfo[0]='\0';

	// get the length of the client info
	ssize_t	result=clientsock->read(&clientinfolen);
	if (result!=sizeof(uint64_t)) {
		clientinfolen=0;
		cont->logClientProtocolError(cursor,
				"get client info failed: "
				"failed to get clientinfo length",result);
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

		cont->debugstr.clear();
		cont->debugstr.append("get client info failed: "
				"client sent bad client info size: ");
		cont->debugstr.append(clientinfolen);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);

		clientinfolen=0;
		return false;
	}

	// read the client info into the buffer
	result=clientsock->read(clientinfo,clientinfolen);
	if ((uint64_t)result!=clientinfolen) {
		clientinfolen=0;
		clientinfo[0]='\0';
		cont->logClientProtocolError(cursor,
				"get client info failed: "
				"failed to get client info",result);
		return false;
	}
	clientinfo[clientinfolen]='\0';

	if (sqlrlg) {
		cont->debugstr.clear();
		cont->debugstr.append("clientinfolen: ")->append(clientinfolen);
		cont->logDebugMessage(cont->debugstr.getString());
		cont->debugstr.clear();
		cont->debugstr.append("clientinfo: ")->append(clientinfo);
		cont->logDebugMessage(cont->debugstr.getString());
		cont->logDebugMessage("getting clientinfo succeeded");
	}

	// update the stats with the client info
	updateClientInfo(clientinfo,clientinfolen);

	return true;
}

bool sqlrcontroller_svr::getQuery(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting query...");

	// init
	cursor->querylength=0;
	cursor->querybuffer[0]='\0';

	// get the length of the query
	ssize_t	result=clientsock->read(&cursor->querylength,
						cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cursor->querylength=0;
		cont->logClientProtocolError(cursor,
				"get query failed: "
				"failed to get query length",result);
		return false;
	}

	// bounds checking
	if (cursor->querylength>maxquerysize) {

		stringbuffer	err;
		err.append(SQLR_ERROR_MAXQUERYLENGTH_STRING);
		err.append(" (")->append(cursor->querylength)->append('>');
		err.append(maxquerysize)->append(')');
		cursor->setError(err.getString(),
				SQLR_ERROR_MAXQUERYLENGTH,true);

		cont->debugstr.clear();
		cont->debugstr.append("get query failed: "
				"client sent bad query length: ");
		cont->debugstr.append(cursor->querylength);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);

		cursor->querylength=0;
		return false;
	}

	// read the query into the buffer
	result=clientsock->read(cursor->querybuffer,
				cursor->querylength,
				cont->idleclienttimeout,0);
	if ((uint32_t)result!=cursor->querylength) {

		cursor->querylength=0;
		cursor->querybuffer[0]='\0';

		cont->logClientProtocolError(cursor,
				"get query failed: "
				"failed to get query",result);
		return false;
	}
	cursor->querybuffer[cursor->querylength]='\0';

	if (sqlrlg) {
		cont->debugstr.clear();
		cont->debugstr.append("querylength: ")->append(cursor->querylength);
		cont->logDebugMessage(cont->debugstr.getString());
		cont->debugstr.clear();
		cont->debugstr.append("query: ")->append(cursor->querybuffer);
		cont->logDebugMessage(cont->debugstr.getString());
		cont->logDebugMessage("getting query succeeded");
	}

	// update the stats with the current query
	updateCurrentQuery(cursor->querybuffer,cursor->querylength);

	return true;
}

bool sqlrcontroller_svr::getInputBinds(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting input binds...");

	// get the number of input bind variable/values
	if (!getBindVarCount(cursor,&(cursor->inbindcount))) {
		return false;
	}
	
	// fill the buffers
	for (uint16_t i=0; i<cursor->inbindcount && i<maxbindcount; i++) {

		bindvar_svr	*bv=&(cursor->inbindvars[i]);

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
			cursor->fakeinputbindsforthisquery=false;
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

bool sqlrcontroller_svr::getOutputBinds(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting output binds...");

	// get the number of output bind variable/values
	if (!getBindVarCount(cursor,&(cursor->outbindcount))) {
		return false;
	}

	// fill the buffers
	for (uint16_t i=0; i<cursor->outbindcount && i<maxbindcount; i++) {

		bindvar_svr	*bv=&(cursor->outbindvars[i]);

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
			bv->value.dateval.buffer=(char *)bindpool->allocate(
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
			sqlrcursor_svr	*curs=findAvailableCursor();
			if (!curs) {
				// FIXME: set error here
				return false;
			}
			curs->state=SQLRCURSORSTATE_BUSY;
			bv->value.cursorid=curs->id;
		}

		// init the null indicator
		bv->isnull=conn->nonNullBindValue();
	}

	cont->logDebugMessage("done getting output binds");
	return true;
}

bool sqlrcontroller_svr::getBindVarCount(sqlrcursor_svr *cursor,
						uint16_t *count) {

	// init
	*count=0;

	// get the number of input bind variable/values
	ssize_t	result=clientsock->read(count,cont->idleclienttimeout,0);
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

		cont->debugstr.clear();
		cont->debugstr.append("get binds failed: "
				"client tried to send too many binds: ");
		cont->debugstr.append(*count);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);

		*count=0;
		return false;
	}

	return true;
}

bool sqlrcontroller_svr::getBindVarName(sqlrcursor_svr *cursor,
						bindvar_svr *bv) {

	// init
	bv->variablesize=0;
	bv->variable=NULL;

	// get the variable name size
	uint16_t	bindnamesize;
	ssize_t		result=clientsock->read(&bindnamesize,
						cont->idleclienttimeout,0);
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

		cont->debugstr.clear();
		cont->debugstr.append("get binds failed: bad variable name length: ");
		cont->debugstr.append(bindnamesize);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);
		return false;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)bindmappingspool->allocate(bindnamesize+2);
	bv->variable[0]=conn->bindVariablePrefix();
	result=clientsock->read(bv->variable+1,bindnamesize,
					cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getBindVarType(bindvar_svr *bv) {

	// get the type
	ssize_t	result=clientsock->read(&bv->type,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get type",result);
		return false;
	}
	return true;
}

bool sqlrcontroller_svr::getBindSize(sqlrcursor_svr *cursor,
					bindvar_svr *bv, uint32_t *maxsize) {

	// init
	bv->valuesize=0;

	// get the size of the value
	ssize_t	result=clientsock->read(&(bv->valuesize),cont->idleclienttimeout,0);
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
		cont->debugstr.clear();
		cont->debugstr.append("get binds failed: bad value length: ");
		cont->debugstr.append(bv->valuesize);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);
		return false;
	}

	return true;
}

void sqlrcontroller_svr::getNullBind(bindvar_svr *bv) {

	cont->logDebugMessage("NULL");

	bv->value.stringval=(char *)bindpool->allocate(1);
	bv->value.stringval[0]='\0';
	bv->valuesize=0;
	bv->isnull=conn->nullBindValue();
}

bool sqlrcontroller_svr::getStringBind(sqlrcursor_svr *cursor,
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
					cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getIntegerBind(bindvar_svr *bv) {

	cont->logDebugMessage("INTEGER");

	// get the value itself
	uint64_t	value;
	ssize_t		result=clientsock->read(&value,cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getDoubleBind(bindvar_svr *bv) {

	cont->logDebugMessage("DOUBLE");

	// get the value
	ssize_t	result=clientsock->read(&(bv->value.doubleval.value),
						cont->idleclienttimeout,0);
	if (result!=sizeof(double)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get bind value",result);
		return false;
	}

	// get the precision
	result=clientsock->read(&(bv->value.doubleval.precision),
						cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get precision",result);
		return false;
	}

	// get the scale
	result=clientsock->read(&(bv->value.doubleval.scale),
						cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getDateBind(bindvar_svr *bv) {

	cont->logDebugMessage("DATE");

	// init
	bv->value.dateval.tz=NULL;

	uint16_t	temp;

	// get the year
	ssize_t	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get year",result);
		return false;
	}
	bv->value.dateval.year=(int16_t)temp;

	// get the month
	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get month",result);
		return false;
	}
	bv->value.dateval.month=(int16_t)temp;

	// get the day
	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get day",result);
		return false;
	}
	bv->value.dateval.day=(int16_t)temp;

	// get the hour
	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get hour",result);
		return false;
	}
	bv->value.dateval.hour=(int16_t)temp;

	// get the minute
	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get minute",result);
		return false;
	}
	bv->value.dateval.minute=(int16_t)temp;

	// get the second
	result=clientsock->read(&temp,cont->idleclienttimeout,0);
	if (result!=sizeof(uint16_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get second",result);
		return false;
	}
	bv->value.dateval.second=(int16_t)temp;

	// get the microsecond
	uint32_t	temp32;
	result=clientsock->read(&temp32,cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(NULL,
				"get binds failed: "
				"failed to get microsecond",result);
		return false;
	}
	bv->value.dateval.microsecond=(int32_t)temp32;

	// get the size of the time zone
	uint16_t	length;
	result=clientsock->read(&length,cont->idleclienttimeout,0);
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
					cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getLobBind(sqlrcursor_svr *cursor, bindvar_svr *bv) {

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
					cont->idleclienttimeout,0);
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

bool sqlrcontroller_svr::getSendColumnInfo() {

	cont->logDebugMessage("get send column info...");

	ssize_t	result=clientsock->read(&sendcolumninfo,cont->idleclienttimeout,0);
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

	return true;
}

bool sqlrcontroller_svr::getSkipAndFetch(sqlrcursor_svr *cursor) {

	// get the number of rows to skip
	ssize_t		result=clientsock->read(&skip,cont->idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(cursor,
				"return result set data failed: "
				"failed to get rows to skip",result);
		return false;
	}

	// get the number of rows to fetch
	result=clientsock->read(&fetch,cont->idleclienttimeout,0);
	if (result!=sizeof(uint64_t)) {
		cont->logClientProtocolError(cursor,
				"return result set data failed: "
				"failed to get rows to fetch",result);
		return false;
	}
	return true;
}

void sqlrcontroller_svr::returnResultSetHeader(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("returning result set header...");

	// return the row counts
	cont->logDebugMessage("returning row counts...");
	sendRowCounts(cursor->knowsRowCount(),cursor->rowCount(),
			cursor->knowsAffectedRows(),cursor->affectedRows());
	cont->logDebugMessage("done returning row counts");


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

void sqlrcontroller_svr::returnColumnInfo(sqlrcursor_svr *cursor,
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

void sqlrcontroller_svr::sendRowCounts(bool knowsactual, uint64_t actual,
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

void sqlrcontroller_svr::returnOutputBindValues(sqlrcursor_svr *cursor) {

	if (sqlrlg) {
		cont->debugstr.clear();
		cont->debugstr.append("returning ");
		cont->debugstr.append(cursor->outbindcount);
		cont->debugstr.append(" output bind values: ");
		cont->logDebugMessage(cont->debugstr.getString());
	}

	// run through the output bind values, sending them back
	for (uint16_t i=0; i<cursor->outbindcount; i++) {

		bindvar_svr	*bv=&(cursor->outbindvars[i]);

		if (sqlrlg) {
			cont->debugstr.clear();
			cont->debugstr.append(i);
			cont->debugstr.append(":");
		}

		if (conn->bindValueIsNull(bv->isnull)) {

			if (sqlrlg) {
				cont->debugstr.append("NULL");
			}

			clientsock->write((uint16_t)NULL_DATA);

		} else if (bv->type==BLOB_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("BLOB:");
			}

			returnOutputBindBlob(cursor,i);

		} else if (bv->type==CLOB_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("CLOB:");
			}

			returnOutputBindClob(cursor,i);

		} else if (bv->type==STRING_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("STRING:");
				cont->debugstr.append(bv->value.stringval);
			}

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==INTEGER_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("INTEGER:");
				cont->debugstr.append(bv->value.integerval);
			}

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==DOUBLE_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("DOUBLE:");
				cont->debugstr.append(bv->value.doubleval.value);
				cont->debugstr.append("(");
				cont->debugstr.append(bv->value.doubleval.precision);
				cont->debugstr.append(",");
				cont->debugstr.append(bv->value.doubleval.scale);
				cont->debugstr.append(")");
			}

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==DATE_BIND) {

			if (sqlrlg) {
				cont->debugstr.append("DATE:");
				cont->debugstr.append(bv->value.dateval.year);
				cont->debugstr.append("-");
				cont->debugstr.append(bv->value.dateval.month);
				cont->debugstr.append("-");
				cont->debugstr.append(bv->value.dateval.day);
				cont->debugstr.append(" ");
				cont->debugstr.append(bv->value.dateval.hour);
				cont->debugstr.append(":");
				cont->debugstr.append(bv->value.dateval.minute);
				cont->debugstr.append(":");
				cont->debugstr.append(bv->value.dateval.second);
				cont->debugstr.append(":");
				cont->debugstr.append(bv->value.dateval.microsecond);
				cont->debugstr.append(" ");
				cont->debugstr.append(bv->value.dateval.tz);
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

			if (sqlrlg) {
				cont->debugstr.append("CURSOR:");
				cont->debugstr.append(bv->value.cursorid);
			}

			clientsock->write((uint16_t)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		if (sqlrlg) {
			cont->logDebugMessage(cont->debugstr.getString());
		}
	}

	cont->logDebugMessage("done returning output bind values");
}

void sqlrcontroller_svr::returnOutputBindBlob(sqlrcursor_svr *cursor,
							uint16_t index) {
	sendLobOutputBind(cursor,index);
}

void sqlrcontroller_svr::returnOutputBindClob(sqlrcursor_svr *cursor,
							uint16_t index) {
	sendLobOutputBind(cursor,index);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrcontroller_svr::sendLobOutputBind(sqlrcursor_svr *cursor,
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
	uint64_t	charstoread=sizeof(cursor->lobbuffer)/
						MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cursor->getLobOutputBindSegment(
					index,
					cursor->lobbuffer,
					sizeof(cursor->lobbuffer),
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
			sendLongSegment(cursor->lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrcontroller_svr::sendColumnDefinition(const char *name,
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

	if (sqlrlg) {
		cont->debugstr.clear();
		for (uint16_t i=0; i<namelen; i++) {
			cont->debugstr.append(name[i]);
		}
		cont->debugstr.append(":");
		cont->debugstr.append(type);
		cont->debugstr.append(":");
		cont->debugstr.append(size);
		cont->debugstr.append(" (");
		cont->debugstr.append(precision);
		cont->debugstr.append(",");
		cont->debugstr.append(scale);
		cont->debugstr.append(") ");
		if (!nullable) {
			cont->debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			cont->debugstr.append("Primary key ");
		}
		if (unique) {
			cont->debugstr.append("Unique");
		}
		cont->logDebugMessage(cont->debugstr.getString());
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

void sqlrcontroller_svr::sendColumnDefinitionString(const char *name,
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

	if (sqlrlg) {
		cont->debugstr.clear();
		for (uint16_t ni=0; ni<namelen; ni++) {
			cont->debugstr.append(name[ni]);
		}
		cont->debugstr.append(":");
		for (uint16_t ti=0; ti<typelen; ti++) {
			cont->debugstr.append(type[ti]);
		}
		cont->debugstr.append(":");
		cont->debugstr.append(size);
		cont->debugstr.append(" (");
		cont->debugstr.append(precision);
		cont->debugstr.append(",");
		cont->debugstr.append(scale);
		cont->debugstr.append(") ");
		if (!nullable) {
			cont->debugstr.append("NOT NULL ");
		}
		if (primarykey) {
			cont->debugstr.append("Primary key ");
		}
		if (unique) {
			cont->debugstr.append("Unique");
		}
		cont->logDebugMessage(cont->debugstr.getString());
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

bool sqlrcontroller_svr::returnResultSetData(sqlrcursor_svr *cursor,
						bool getskipandfetch) {

	cont->logDebugMessage("returning result set data...");

	cont->updateState(RETURN_RESULT_SET);

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->customquerycursor) {
		cursor=cursor->customquerycursor;
	}

	// get the number of rows to skip and fetch
	if (getskipandfetch) {
		if (!getSkipAndFetch(cursor)) {
			return false;
		}
	}

	// reinit cursor state (in case it was suspended)
	cursor->state=SQLRCURSORSTATE_BUSY;

	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((uint16_t)END_RESULT_SET);
		clientsock->flushWriteBuffer(-1,-1);
		cont->logDebugMessage("done returning result set data");
		return true;
	}

	// skip the specified number of rows
	if (!skipRows(cursor,skip)) {
		clientsock->write((uint16_t)END_RESULT_SET);
		clientsock->flushWriteBuffer(-1,-1);
		cont->logDebugMessage("done returning result set data");
		return true;
	}


	if (sqlrlg) {
		cont->debugstr.clear();
		cont->debugstr.append("fetching ");
		cont->debugstr.append(fetch);
		cont->debugstr.append(" rows...");
		cont->logDebugMessage(cont->debugstr.getString());
	}

	// send the specified number of rows back
	for (uint64_t i=0; (!fetch || i<fetch); i++) {

		if (!cursor->fetchRow()) {
			clientsock->write((uint16_t)END_RESULT_SET);
			clientsock->flushWriteBuffer(-1,-1);
			cont->logDebugMessage("done returning result set data");
			return true;
		}

		if (sqlrlg) {
			cont->debugstr.clear();
		}

		returnRow(cursor);

		if (sqlrlg) {
			cont->logDebugMessage(cont->debugstr.getString());
		}

		if (cursor->lastrowvalid) {
			cursor->lastrow++;
		} else {
			cursor->lastrowvalid=true;
			cursor->lastrow=0;
		}
	}
	clientsock->flushWriteBuffer(-1,-1);

	cont->logDebugMessage("done returning result set data");
	return true;
}

void sqlrcontroller_svr::sendField(sqlrcursor_svr *cursor,
					uint32_t index,
					const char *data,
					uint32_t size) {

	// convert date/time values, if configured to do so
	if (reformatdatetimes) {

		// are dates going to be in MM/DD or DD/MM format?
		bool	ddmm=cfgfl->getDateDdMm();
		bool	yyyyddmm=cfgfl->getDateYyyyDdMm();

		// This weirdness is mainly to address a FreeTDS/MSSQL
		// issue.  See the code for the method
		// freetdscursor::ignoreDateDdMmParameter() for more info.
		if (cursor->ignoreDateDdMmParameter(index,data,size)) {
			ddmm=false;
			yyyyddmm=false;
		}

		int16_t	year=-1;
		int16_t	month=-1;
		int16_t	day=-1;
		int16_t	hour=-1;
		int16_t	minute=-1;
		int16_t	second=-1;
		int16_t	fraction=-1;
		if (parseDateTime(data,ddmm,yyyyddmm,true,
					&year,&month,&day,
					&hour,&minute,&second,
					&fraction)) {

			// decide which format to use based on what parts
			// were detected in the date/time
			const char	*format=cfgfl->getDateTimeFormat();
			if (hour==-1) {
				format=cfgfl->getDateFormat();
			} else if (day==-1) {
				format=cfgfl->getTimeFormat();
			}

			// convert to the specified format
			char	*newdata=convertDateTime(format,
							year,month,day,
							hour,minute,second,
							fraction);

			// send the field
			sendField(newdata,charstring::length(newdata));

			if (debugsqltranslation) {
				stdoutput.printf("converted date: "
					"\"%s\" to \"%s\" using ddmm=%d\n",
					data,newdata,ddmm);
			}

			// clean up
			delete[] newdata;
			return;
		}
	}

	// send the field normally
	sendField(data,size);
}

void sqlrcontroller_svr::sendField(const char *data, uint32_t size) {

	if (sqlrlg) {
		cont->debugstr.append("\"");
		cont->debugstr.append(data,size);
		cont->debugstr.append("\",");
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrcontroller_svr::sendNullField() {

	if (sqlrlg) {
		cont->debugstr.append("NULL");
	}

	clientsock->write((uint16_t)NULL_DATA);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrcontroller_svr::sendLobField(sqlrcursor_svr *cursor, uint32_t col) {

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
	uint64_t	charstoread=sizeof(cursor->lobbuffer)/
						MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cursor->getLobFieldSegment(col,
					cursor->lobbuffer,
					sizeof(cursor->lobbuffer),
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
			sendLongSegment(cursor->lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrcontroller_svr::startSendingLong(uint64_t longlength) {
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrcontroller_svr::sendLongSegment(const char *data, uint32_t size) {

	if (sqlrlg) {
		cont->debugstr.append(data,size);
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrcontroller_svr::endSendingLong() {

	if (sqlrlg) {
		cont->debugstr.append(",");
	}

	clientsock->write((uint16_t)END_LONG_DATA);
}

void sqlrcontroller_svr::returnError(bool disconnect) {

	// Get the error data if none is set already
	if (!conn->error) {
		conn->errorMessage(conn->error,maxerrorlength,
				&conn->errorlength,&conn->errnum,
				&conn->liveconnection);
		if (!conn->liveconnection) {
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
	clientsock->write((uint64_t)conn->errnum);

	// send the error string
	clientsock->write((uint16_t)conn->errorlength);
	clientsock->write(conn->error,conn->errorlength);
	clientsock->flushWriteBuffer(-1,-1);

	logDbError(NULL,conn->error);
}

void sqlrcontroller_svr::returnError(sqlrcursor_svr *cursor, bool disconnect) {

	cont->logDebugMessage("returning error...");

	// send the appropriate error status
	if (disconnect) {
		clientsock->write((uint16_t)ERROR_OCCURRED_DISCONNECT);
	} else {
		clientsock->write((uint16_t)ERROR_OCCURRED);
	}

	// send the error code
	clientsock->write((uint64_t)cursor->errnum);

	// send the error string
	clientsock->write((uint16_t)cursor->errorlength);
	clientsock->write(cursor->error,cursor->errorlength);

	// client will be sending skip/fetch, better get
	// it even though we're not going to use it
	uint64_t	skipfetch;
	clientsock->read(&skipfetch,cont->idleclienttimeout,0);
	clientsock->read(&skipfetch,cont->idleclienttimeout,0);

	// Even though there was an error, we still 
	// need to send the client the id of the 
	// cursor that it's going to use.
	clientsock->write(cursor->id);
	clientsock->flushWriteBuffer(-1,-1);

	cont->logDebugMessage("done returning error");

	logDbError(cursor,cursor->error);
}

bool sqlrcontroller_svr::fetchResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("fetching result set...");
	bool	retval=returnResultSetData(cursor,true);
	cont->logDebugMessage("done fetching result set");
	return retval;
}

void sqlrcontroller_svr::abortResultSetCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("aborting result set...");

	// Very important...
	// Do not cleanUpData() here, otherwise result sets that were suspended
	// after the entire result set was fetched won't be able to return
	// column data when resumed.
	cursor->abort();

	cont->logDebugMessage("done aborting result set");
}

void sqlrcontroller_svr::suspendResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("suspend result set...");
	cursor->state=SQLRCURSORSTATE_SUSPENDED;
	if (cursor->customquerycursor) {
		cursor->customquerycursor->state=SQLRCURSORSTATE_SUSPENDED;
	}
	cont->logDebugMessage("done suspending result set");
}

bool sqlrcontroller_svr::resumeResultSetCommand(sqlrcursor_svr *cursor) {
	cont->logDebugMessage("resume result set...");

	bool	retval=true;

	if (cursor->state==SQLRCURSORSTATE_SUSPENDED) {

		cont->logDebugMessage("previous result set was suspended");

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR_OCCURRED);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cursor->id);
		clientsock->write((uint16_t)SUSPENDED_RESULT_SET);

		// if the requested cursor really had a suspended
		// result set, send the lastrow of it to the client
		// then resume the result set
		clientsock->write(cursor->lastrow);

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

bool sqlrcontroller_svr::getListCommand(sqlrcursor_svr *cursor,
						int which, bool gettable) {

	// clean up any custom query cursors
	if (cursor->customquerycursor) {
		cursor->customquerycursor->close();
		delete cursor->customquerycursor;
		cursor->customquerycursor=NULL;
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData();

	// get length of wild parameter
	uint32_t	wildlen;
	ssize_t		result=clientsock->read(&wildlen,cont->idleclienttimeout,0);
	if (result!=sizeof(uint32_t)) {
		cont->logClientProtocolError(cursor,
				"get list failed: "
				"failed to get wild length",result);
		return false;
	}

	// bounds checking
	if (wildlen>maxquerysize) {
		cont->debugstr.clear();
		cont->debugstr.append("get list failed: wild length too large: ");
		cont->debugstr.append(wildlen);
		cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);
		return false;
	}

	// read the wild parameter into the buffer
	char	*wild=new char[wildlen+1];
	if (wildlen) {
		result=clientsock->read(wild,wildlen,cont->idleclienttimeout,0);
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
		result=clientsock->read(&tablelen,cont->idleclienttimeout,0);
		if (result!=sizeof(uint32_t)) {
			cont->logClientProtocolError(cursor,
					"get list failed: "
					"failed to get table length",result);
			return false;
		}

		// bounds checking
		if (tablelen>maxquerysize) {
			cont->debugstr.clear();
			cont->debugstr.append("get list failed: "
					"table length too large: ");
			cont->debugstr.append(tablelen);
			cont->logClientProtocolError(cursor,cont->debugstr.getString(),1);
			return false;
		}

		// read the table parameter into the buffer
		table=new char[tablelen+1];
		if (tablelen) {
			result=clientsock->read(table,tablelen,
						cont->idleclienttimeout,0);
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
		if (sqlt) {
			const char	*newname=NULL;
			if (sqlt->getReplacementTableName(NULL,NULL,
							table,&newname)) {
				delete[] table;
				table=charstring::duplicate(newname);
			}
		}
	}

	// set the values that we won't get from the client
	cursor->inbindcount=0;
	cursor->outbindcount=0;
	sendcolumninfo=SEND_COLUMN_INFO;

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

bool sqlrcontroller_svr::getListByApiCall(sqlrcursor_svr *cursor,
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
		cursor->errorMessage(cursor->error,
					maxerrorlength,
					&(cursor->errorlength),
					&(cursor->errnum),
					&(cursor->liveconnection));
		returnError(cursor,!cursor->liveconnection);

		// this is actually OK, only return false on a network error
		return true;
	}

	// indicate that no error has occurred
	clientsock->write((uint16_t)NO_ERROR_OCCURRED);

	// send the client the id of the 
	// cursor that it's going to use
	clientsock->write(cursor->id);

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

bool sqlrcontroller_svr::getListByQuery(sqlrcursor_svr *cursor,
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

	// run it like a normal query, but don't request the query,
	// binds or column info status from the client
	return handleQueryOrBindCursor(cursor,false,false,false);
}

bool sqlrcontroller_svr::buildListQuery(sqlrcursor_svr *cursor,
						const char *query,
						const char *wild,
						const char *table) {

	// clean up buffers to avoid SQL injection
	stringbuffer	wildbuf;
	escapeParameter(&wildbuf,wild);
	stringbuffer	tablebuf;
	escapeParameter(&tablebuf,table);

	// bounds checking
	cursor->querylength=charstring::length(query)+
					wildbuf.getStringLength()+
					tablebuf.getStringLength();
	if (cursor->querylength>maxquerysize) {
		return false;
	}

	// fill the query buffer and update the length
	if (tablebuf.getStringLength()) {
		charstring::printf(cursor->querybuffer,maxquerysize+1,
						query,tablebuf.getString(),
						wildbuf.getString());
	} else {
		charstring::printf(cursor->querybuffer,maxquerysize+1,
						query,wildbuf.getString());
	}
	cursor->querylength=charstring::length(cursor->querybuffer);
	return true;
}

void sqlrcontroller_svr::escapeParameter(stringbuffer *buffer,
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

bool sqlrcontroller_svr::getQueryTreeCommand(sqlrcursor_svr *cursor) {

	cont->logDebugMessage("getting query tree");

	// get the tree as a string
	xmldomnode	*tree=(cursor->querytree)?
				cursor->querytree->getRootNode():NULL;
	stringbuffer	*xml=(tree)?tree->xml():NULL;
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

void sqlrcontroller_svr::closeClientSocket() {

	// Sometimes the server sends the result set and closes the socket
	// while part of it is buffered but not yet transmitted.  This causes
	// the client to receive a partial result set or error.  Telling the
	// socket to linger doesn't always fix it.  Doing a read here should 
	// guarantee that the client will close its end of the connection 
	// before the server closes its end; the server will wait for data 
	// from the client (which it will never receive) and when the client 
	// closes its end (which it will only do after receiving the entire
	// result set) the read will fall through.  This should guarantee 
	// that the client will get the the entire result set without
	// requiring the client to send data back indicating so.
	//
	// Also, if authentication fails, the client could send an entire query
	// and bind vars before it reads the error and closes the socket.
	// We have to absorb all of that data.  We shouldn't just loop forever
	// though, that would provide a point of entry for a DOS attack.  We'll
	// read the maximum number of bytes that could be sent.
	cont->logDebugMessage("waiting for client to close the connection...");
	uint16_t	dummy;
	uint32_t	counter=0;
	clientsock->useNonBlockingMode();
	while (clientsock->read(&dummy,cont->idleclienttimeout,0)>0 &&
				counter<
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
				)/2) {
		counter++;
	}
	clientsock->useBlockingMode();
	
	cont->logDebugMessage("done waiting for client to close the connection");

	// close the client socket
	cont->logDebugMessage("closing the client socket...");
	if (proxymode) {

		cont->logDebugMessage("(actually just signalling the listener)");

		// we do need to signal the proxy that it
		// needs to close the connection though
		signalmanager::sendSignal(proxypid,SIGUSR1);

		// in proxy mode, the client socket is pointed at the
		// handoff socket which we don't want to actually close
		clientsock->setFileDescriptor(-1);
	}
	clientsock->close();
	delete clientsock;
	cont->logDebugMessage("done closing the client socket");
}