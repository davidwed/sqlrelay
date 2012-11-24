// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>
#include <rudiments/rawbuffer.h>

// for gettimeofday()
#include <sys/time.h>

bool sqlrcontroller_svr::handleQueryOrBindCursor(sqlrcursor_svr *cursor,
							bool reexecute,
							bool bindcursor,
							bool getquery) {


	dbgfile.debugPrint("connection",1,"handling query...");

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->customquerycursor) {
		if (reexecute) {
			cursor=cursor->customquerycursor;
		} else {
			delete cursor->customquerycursor;
			cursor->customquerycursor=NULL;
		}
	}

	// re-init error data
	cursor->clearError();

	// clear bind mappings and reset the fake input binds flag
	// (do this here because getInput/OutputBinds uses the bindmappingspool)
	if (!bindcursor && !reexecute) {
		bindmappingspool->free();
		inbindmappings->clear();
		outbindmappings->clear();
		cursor->fakeinputbindsforthisquery=fakeinputbinds;
	}

	// clean up whatever result set the cursor might have been busy with
	cursor->cleanUpData(true,true);

	// get the query and bind data from the client...
	if (getquery) {

		// re-init buffers
		if (!reexecute && !bindcursor) {
			clientinfo[0]='\0';
			clientinfolen=0;
			cursor->querybuffer[0]='\0';
			cursor->querylength=0;
		}
		if (!bindcursor) {
			cursor->inbindcount=0;
			cursor->outbindcount=0;
			for (uint16_t i=0; i<maxbindcount; i++) {
				rawbuffer::zero(&(cursor->inbindvars[i]),
							sizeof(bindvar_svr));
				rawbuffer::zero(&(cursor->outbindvars[i]),
							sizeof(bindvar_svr));
			}
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

				// copy the query that we just got into the
				// custom query cursor
				charstring::copy(
					cursor->customquerycursor->querybuffer,
					cursor->querybuffer);
				cursor->customquerycursor->querylength=
							cursor->querylength;

				// set the cursor state
				cursor->customquerycursor->state=
						SQLRCURSOR_STATE_BUSY;

				// reset the rest of this method to use
				// the custom query cursor
				cursor=cursor->customquerycursor;
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
			dbgfile.debugPrint("connection",1,
						"failed to handle query");
			return false;
		}
	}

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
			bindpool->free();

			dbgfile.debugPrint("connection",1,
						"handle query succeeded");

			// reinit lastrow
			cursor->lastrowvalid=false;

			// return the result set
			return returnResultSetData(cursor);

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

				dbgfile.debugPrint("connection",3,
							"database is down...");
				reLogIn();

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

	dbgfile.debugPrint("connection",2,"getting client info...");

	// init
	clientinfolen=0;
	clientinfo[0]='\0';

	// get the length of the client info
	if (clientsock->read(&clientinfolen)!=sizeof(uint64_t)) {
		dbgfile.debugPrint("connection",2,
			"getting client info failed: "
			"client sent bad client info size");
		clientinfolen=0;
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

		clientinfolen=0;

		dbgfile.debugPrint("connection",2,
			"getting client info failed: "
			"client sent bad client info size");
		return false;
	}

	// read the client info into the buffer
	if (clientsock->read(clientinfo,clientinfolen)!=clientinfolen) {
		dbgfile.debugPrint("connection",2,
			"getting client info failed: "
			"client sent short client info");
		clientinfolen=0;
		clientinfo[0]='\0';
		return false;
	}
	clientinfo[clientinfolen]='\0';

	dbgfile.debugPrint("connection",3,"clientinfolen:");
	dbgfile.debugPrint("connection",4,(int32_t)clientinfolen);
	dbgfile.debugPrint("connection",3,"clientinfo:");
	dbgfile.debugPrint("connection",0,clientinfo);
	dbgfile.debugPrint("connection",2,"getting clientinfo succeeded");

	return true;
}

bool sqlrcontroller_svr::getQuery(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"getting query...");

	// init
	cursor->querylength=0;
	cursor->querybuffer[0]='\0';

	// get the length of the query
	if (clientsock->read(&cursor->querylength,
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		cursor->querylength=0;
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

		cursor->querylength=0;

		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		return false;
	}

	// read the query into the buffer
	if (clientsock->read(cursor->querybuffer,cursor->querylength,
				idleclienttimeout,0)!=cursor->querylength) {
		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent short query");
		cursor->querylength=0;
		cursor->querybuffer[0]='\0';
		return false;
	}
	cursor->querybuffer[cursor->querylength]='\0';

	dbgfile.debugPrint("connection",3,"querylength:");
	dbgfile.debugPrint("connection",4,(int32_t)cursor->querylength);
	dbgfile.debugPrint("connection",3,"query:");
	dbgfile.debugPrint("connection",0,cursor->querybuffer);
	dbgfile.debugPrint("connection",2,"getting query succeeded");

	return true;
}

bool sqlrcontroller_svr::getSendColumnInfo() {

	dbgfile.debugPrint("connection",2,"getting send column info...");

	if (clientsock->read(&sendcolumninfo,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting send column info failed");
		return false;
	}

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		dbgfile.debugPrint("connection",3,"send column info");
	} else {
		dbgfile.debugPrint("connection",3,"don't send column info");
	}
	dbgfile.debugPrint("connection",2,"done getting send column info...");

	return true;
}

bool sqlrcontroller_svr::processQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor) {

	dbgfile.debugPrint("connection",2,"processing query...");

	// on reexecute, translate bind variables from mapping
	if (reexecute) {
		translateBindVariablesFromMappings(cursor);
	}

	bool	success=false;
	bool	supportsnativebinds=cursor->supportsNativeBinds();

	if (reexecute &&
		!cursor->fakeinputbindsforthisquery &&
		supportsnativebinds) {

		// if we're reexecuting and not faking binds then
		// the statement doesn't need to be prepared again,
		// just execute it...

		dbgfile.debugPrint("connection",3,"re-executing...");
		success=(handleBinds(cursor) && executeQuery(cursor,
							cursor->querybuffer,
							cursor->querylength));

	} else if (bindcursor) {

		// if we're handling a bind cursor...
		dbgfile.debugPrint("connection",3,"bind cursor...");
		success=cursor->fetchFromBindCursor();

	} else {

		// otherwise, prepare and execute the query...
		// generally this a first time query but it could also be
		// a reexecute if we're faking binds

		dbgfile.debugPrint("connection",3,"preparing/executing...");

		// rewrite the query, if necessary
		rewriteQuery(cursor);

		// buffers and pointers...
		stringbuffer	outputquery;
		const char	*query=cursor->querybuffer;
		uint32_t	querylen=cursor->querylength;

		// fake input binds if necessary
		// NOTE: we can't just overwrite the querybuffer when
		// faking binds or we'll lose the original query and
		// end up rerunning the modified query when reexecuting
		if (cursor->fakeinputbindsforthisquery ||
					!supportsnativebinds) {
			dbgfile.debugPrint("connection",3,"faking binds...");
			if (cursor->fakeInputBinds(&outputquery)) {
				query=outputquery.getString();
				querylen=outputquery.getStringLength();
			}
		}

		// prepare
		success=cursor->prepareQuery(query,querylen);

		// if we're not faking binds then
		// handle the binds for real
		if (success &&
			!cursor->fakeinputbindsforthisquery &&
			cursor->supportsNativeBinds()) {
			success=handleBinds(cursor);
		}

		// execute
		if (success) {
			success=executeQuery(cursor,query,querylen);
		}
	}

	// was the query a commit or rollback?
	commitOrRollback(cursor);

	// On success, autocommit if necessary.
	// Connection classes could override autoCommitOn() and autoCommitOff()
	// to do database API-specific things, but will not set 
	// fakeautocommit, so this code won't get called at all for those 
	// connections.
	// FIXME: when faking autocommit, a BEGIN on a db that supports them
	// could cause commit to be called immediately
	if (success && conn->isTransactional() && commitorrollback &&
				conn->fakeautocommit && conn->autocommit) {
		dbgfile.debugPrint("connection",3,"commit necessary...");
		success=commit();
	}
	
	// if the query failed, get the error (unless it's already been set)
	if (!success && !cursor->errnum) {
		// FIXME: errors for queries run by triggers won't be set here
		cursor->errorMessage(cursor->error,
					maxerrorlength,
					&(cursor->errorlength),
					&(cursor->errnum),
					&(cursor->liveconnection));
	}

	if (success) {
		dbgfile.debugPrint("connection",2,"processing query succeeded");
	} else {
		dbgfile.debugPrint("connection",2,"processing query failed");
	}
	dbgfile.debugPrint("connection",2,"done processing query");


	return success;
}

bool sqlrcontroller_svr::executeQuery(sqlrcursor_svr *curs,
						const char *query,
						uint32_t length) {

	// handle before-triggers
	if (sqltr) {
		sqltr->runBeforeTriggers(conn,curs,curs->querytree);
	}

	// update query count
	semset->waitWithUndo(9);
	statistics->total_queries++;
	semset->signalWithUndo(9);

	// variables for query timing
	timeval		tv;
	struct timezone	tz;

	// get the query start time
	gettimeofday(&tv,&tz);
	curs->querystartsec=tv.tv_sec;
	curs->querystartusec=tv.tv_usec;

	// execute the query
	curs->queryresult=curs->executeQuery(query,length);

	// get the query end time
	gettimeofday(&tv,&tz);
	curs->queryendsec=tv.tv_sec;
	curs->queryendusec=tv.tv_usec;

	// update error count
	if (!curs->queryresult) {
		semset->waitWithUndo(9);
		statistics->total_errors++;
		semset->signalWithUndo(9);
	}

	// handle after-triggers
	if (sqltr) {
		sqltr->runAfterTriggers(conn,curs,curs->querytree,true);
	}

	return curs->queryresult;
}

void sqlrcontroller_svr::commitOrRollback(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"commit or rollback check...");

	// if the query was a commit or rollback, set a flag indicating so
	if (conn->isTransactional()) {
		if (cursor->queryIsCommitOrRollback()) {
			dbgfile.debugPrint("connection",3,
					"commit or rollback not needed");
			commitorrollback=false;
		} else if (cursor->queryIsNotSelect()) {
			dbgfile.debugPrint("connection",3,
					"commit or rollback needed");
			commitorrollback=true;
		}
	}

	dbgfile.debugPrint("connection",2,"done with commit or rollback check");
}

void sqlrcontroller_svr::setFakeInputBinds(bool fake) {
	fakeinputbinds=fake;
}
