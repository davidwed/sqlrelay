// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int32_t sqlrconnection_svr::handleQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute, bool getquery) {


	dbgfile.debugPrint("connection",1,"handling query...");

	if (getquery) {
		if (!getQueryFromClient(cursor,reexecute,bindcursor)) {
			dbgfile.debugPrint("connection",1,
						"failed to handle query");
			return 0;
		}
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

	bool	success=false;
	bool	doegress=true;
	if (reexecute) {

		// if the reexecute flag is set, the query doesn't
		// need to be prepared again...

		dbgfile.debugPrint("connection",3,"re-executing...");
		if (cursor->supportsNativeBinds()) {
			if (cursor->sql_injection_detection_ingress(
							cursor->querybuffer)) {
				doegress=false;
				success=true;
			} else {
				success=(cursor->handleBinds() && 
					executeQueryUpdateStats(cursor,
							cursor->querybuffer,
							cursor->querylength,
							reallyexecute));
			}
		} else {
			stringbuffer	*newquery=cursor->fakeInputBinds(
							cursor->querybuffer);
			const char	*queryptr=(newquery)?
						newquery->getString():
						cursor->querybuffer;
			uint32_t	querylen=(newquery)?
						newquery->getStringLength():
						cursor->querylength;
			if (cursor->sql_injection_detection_ingress(queryptr)) {
				doegress=false;
				success=true;
			} else {
				success=executeQueryUpdateStats(cursor,
							queryptr,
							querylen,
							reallyexecute);
			}
			delete newquery;
		}

	} else if (bindcursor) {

		// if the cursor is a bind cursor then we just need to
		// execute, we don't need to worry about binds...

		dbgfile.debugPrint("connection",3,"bind cursor...");
		if (cursor->sql_injection_detection_ingress(
						cursor->querybuffer)) {
			doegress=false;
			success=true;
		} else {
			success=executeQueryUpdateStats(cursor,
							cursor->querybuffer,
							cursor->querylength,
							reallyexecute);
		}

	} else {

		// otherwise, prepare and execute the query...

		dbgfile.debugPrint("connection",3,"preparing/executing...");
		if (cursor->sql_injection_detection_ingress(
						cursor->querybuffer)) {
			doegress=false;
			success=true;
		} else {

			// FIXME: fake binds should be done here too
			// arguably rewriting should occur even if we're
			// faking
			if (cursor->supportsNativeBinds()) {
				rewriteQueryAndBinds(cursor);
			}

			success=cursor->prepareQuery(cursor->querybuffer,
							cursor->querylength);
			if (success) {
				if (cursor->supportsNativeBinds()) {
					success=(cursor->handleBinds() &&
						executeQueryUpdateStats(
							cursor,
							cursor->querybuffer,
							cursor->querylength,
							true));
				} else {
					stringbuffer	*newquery=
						cursor->fakeInputBinds(
							cursor->querybuffer);
					const char	*queryptr=
						(newquery)?
						newquery->getString():
						cursor->querybuffer;
					uint32_t	querylen=
						(newquery)?
						newquery->getStringLength():
						cursor->querylength;
					bool	execquery=true;
					if (queryptr!=cursor->querybuffer) {
						if (cursor->
						sql_injection_detection_ingress(
							cursor->querybuffer)) {
							doegress=false;
							execquery=false;
						}
					}
					if (execquery) {
						success=executeQueryUpdateStats(
								cursor,
								queryptr,
								querylen,true);
					}
					delete newquery;
				}
			}
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
	// checkautocommit, so this code won't get called at all for those 
	// connections.
	if (success && checkautocommit && isTransactional() && 
			performautocommit && commitorrollback) {
		dbgfile.debugPrint("connection",3,"commit necessary...");
		success=commit();
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
