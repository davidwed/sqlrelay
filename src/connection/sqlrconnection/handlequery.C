// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int32_t sqlrconnection_svr::handleQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute, bool getquery) {


	dbgfile.debugPrint("connection",1,"handling query...");

	// clear bind mappings and reset fakeinputbinds flag
	if (!reexecute && !bindcursor) {
		clearBindMappings();
		fakeinputbinds=false;
	}

	if (getquery) {
		if (!getQueryFromClient(cursor,reexecute,bindcursor)) {
			dbgfile.debugPrint("connection",1,
						"failed to handle query");
			return 0;
		}
	}

	// handle fake begins
	// FIXME: do we need to loop to detect downed db's somehow?
	if (!reexecute && !bindcursor && handleFakeBeginTransaction(cursor)) {

		dbgfile.debugPrint("connection",1,"query was fake begin...");

		// indicate that no error has occurred
		clientsock->write((uint16_t)NO_ERROR);

		// send the client the id of the 
		// cursor that it's going to use
		clientsock->write(cursor->id);

		// tell the client that this is not a
		// suspended result set
		clientsock->write((uint16_t)NO_SUSPENDED_RESULT_SET);

		// row counts
		sendRowCounts(cursor->knowsRowCount(),0,
				cursor->knowsAffectedRows(),0);

		// send column info or not
		clientsock->write((uint16_t)DONT_SEND_COLUMN_INFO);

		// column count
		clientsock->write((uint32_t)0);

		// no bind vars
		clientsock->write((uint16_t)END_BIND_VARS);

		dbgfile.debugPrint("connection",1,"done handling query...");

		return 1;
	}

	// loop here to handle down databases
	for (;;) {

		// process the query
		if (processQuery(cursor,reexecute,bindcursor,reallyexecute)) {

			// indicate that no error has occurred
			clientsock->write((uint16_t)NO_ERROR);

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

			dbgfile.debugPrint("connection",1,"handle query succeeded");
			return 1;

		} else {

			// If the query didn't process ok,
			// handle the error.
			// If handleError returns false then the error
			// was a down database that has presumably
			// come back up by now.  Loop back...
			if (handleError(cursor)) {
				return -1;
			}
		}
	}
}

void sqlrconnection_svr::clearBindMappings() {

	// delete the data from the nodes
	bindmappingspool->free();

	// delete the nodes themselves
	inbindmappings->clear();
	outbindmappings->clear();
}

bool sqlrconnection_svr::getQueryFromClient(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor) {

	// if we're not reexecuting and not using a bound cursor, get the query,
	// if we're not using a bound cursor, get the input/output binds,
	// get whether to send column info or not
	return (((reexecute || bindcursor)?true:getQuery(cursor)) &&
		((bindcursor)?true:
		(getInputBinds(cursor) && getOutputBinds(cursor))) &&
		getSendColumnInfo());
}

bool sqlrconnection_svr::getQuery(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"getting query...");

	// get the length of the query
	if (clientsock->read(&cursor->querylength,
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		return false;
	}

	// bounds checking
	if (cursor->querylength>maxquerysize) {
		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		return false;
	}

	// read the query into the buffer
	if ((uint32_t)(clientsock->read(cursor->querybuffer,
						cursor->querylength,
						idleclienttimeout,0))!=
							cursor->querylength) {
		dbgfile.debugPrint("connection",2,
			"getting query failed: client sent short query");
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

bool sqlrconnection_svr::processQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute) {

	// Very important...
	// Clean up data here instead of when aborting a result set, this
	// allows for result sets that were suspended after the entire
	// result set was fetched to still be able to return column data
	// when resumed.
	cursor->cleanUpData(true,true);

	dbgfile.debugPrint("connection",2,"processing query...");

	// on reexecute, translate bind variables from mapping
	if (reexecute) {
		translateBindVariablesFromMappings(cursor);
	}

	bool	success=false;
	bool	doegress=true;

	if (reexecute && !fakeinputbinds && cursor->supportsNativeBinds()) {

		// if we're reexecuting and not faking binds then
		// the statement doesn't need to be prepared again...

		dbgfile.debugPrint("connection",3,"re-executing...");
		success=(cursor->handleBinds() && 
			executeQueryUpdateStats(cursor,
						cursor->querybuffer,
						cursor->querylength,
						reallyexecute));

	} else if (bindcursor) {

		// if the cursor is a bind cursor then we just need to
		// execute, we don't need to worry about binds...

		dbgfile.debugPrint("connection",3,"bind cursor...");
		// FIXME: should we be passing
		// in the querybuffer and length here?
		success=executeQueryUpdateStats(cursor,
						cursor->querybuffer,
						cursor->querylength,
						reallyexecute);

	} else {

		// otherwise, prepare and execute the query...
		// generally this a first time query but it could also be
		// a reexecute if we're faking binds

		dbgfile.debugPrint("connection",3,"preparing/executing...");

		// rewrite the query, if necessary
		rewriteQuery(cursor);

		if (cursor->sql_injection_detection_ingress(
						cursor->querybuffer)) {

			doegress=false;
			success=true;

		} else {

			const char	*queryptr=cursor->querybuffer;
			uint32_t	querylen=cursor->querylength;

			// fake input binds if necessary
			stringbuffer	*newquery=NULL;
			if (fakeinputbinds || !cursor->supportsNativeBinds()) {

				dbgfile.debugPrint("connection",3,
							"faking binds...");

				newquery=cursor->fakeInputBinds(
						cursor->querybuffer);

				queryptr=(newquery)?
						newquery->getString():
						cursor->querybuffer;
				querylen=(newquery)?
						newquery->getStringLength():
						cursor->querylength;
			}

			// prepare
			success=cursor->prepareQuery(queryptr,querylen);

			// if we're not faking binds then
			// handle the binds for real
			if (success && !fakeinputbinds &&
					cursor->supportsNativeBinds()) {
				success=cursor->handleBinds();
			}

			// execute
			if (success) {
				success=executeQueryUpdateStats(
					cursor,queryptr,querylen,true);
			}

			// clean up
			delete newquery;
		}
	}

	if (doegress) {
		cursor->sid_egress=cursor->sql_injection_detection_egress();
	}
	if (cursor->sid_egress) {
		// FIXME: init this to false somewhere!
		cursor->sql_injection_detection=true;
	}

	// was the query a commit or rollback?
	commitOrRollback(cursor);

	// On success, autocommit if necessary.
	// Connection classes could override autoCommitOn() and autoCommitOff()
	// to do database API-specific things, but will not set 
	// fakeautocommit, so this code won't get called at all for those 
	// connections.
	// FIXME: when faking autocommit, a BEGIN on a db that supports them
	// could get commit called immediately committed
	if (success && fakeautocommit &&
		isTransactional() && autocommit && commitorrollback) {
		dbgfile.debugPrint("connection",3,"commit necessary...");
		success=commitInternal();
	}

	if (success) {
		dbgfile.debugPrint("connection",2,"processing query succeeded");
	} else {
		dbgfile.debugPrint("connection",2,"processing query failed");
	}
	dbgfile.debugPrint("connection",2,"done processing query");

	return success;
}

void sqlrconnection_svr::commitOrRollback(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"commit or rollback check...");

	// if the query was a commit or rollback, set a flag indicating so
	if (isTransactional()) {
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
