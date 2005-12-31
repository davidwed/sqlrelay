// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int32_t sqlrconnection_svr::handleQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute) {


	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"handling query...");
	#endif

	if (!getQueryFromClient(cursor,reexecute,bindcursor)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"failed to handle query");
		#endif
		return 0;
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

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"handle query succeeded");
			#endif
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

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting query...");
	#endif

	// get the length of the query
	if (clientsock->read(&cursor->querylength,
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query length size");
		#endif
		return false;
	}

	// bounds checking
	if (cursor->querylength>maxquerysize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		#endif
		return false;
	}

	// read the query into the buffer
	if ((uint32_t)(clientsock->read(cursor->querybuffer,
						cursor->querylength,
						idleclienttimeout,0))!=
							cursor->querylength) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent short query");
		#endif
		return false;
	}
	cursor->querybuffer[cursor->querylength]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"querylength:");
	debugPrint("connection",4,(int32_t)cursor->querylength);
	debugPrint("connection",3,"query:");
	debugPrint("connection",0,cursor->querybuffer);
	debugPrint("connection",2,"getting query succeeded");
	#endif

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
	if (bindcursor) {
		cursor->cleanUpData(false,true);
	} else {
		cursor->cleanUpData(true,true);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"processing query...");
	#endif

	// if the reexecute flag is set, the query doesn't need to be prepared 
	// again.
	bool	success=false;
	if (reexecute) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"re-executing...");
		#endif
		if (supportsNativeBinds()) {
			if (cursor->sql_injection_detection_ingress(
							cursor->querybuffer)) {
				success=true;
			} else {
				success=(cursor->handleBinds() && 
					cursor->executeQuery(
							cursor->querybuffer,
							cursor->querylength,
							reallyexecute));
			}
			cursor->sid_egress_success=
				cursor->sql_injection_detection_egress();
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
				success=true;
			} else {
				success=cursor->executeQuery(queryptr,
							querylen,
							reallyexecute);
			}
			cursor->sid_egress_success=
				cursor->sql_injection_detection_egress();
			delete newquery;
		}
	} else if (bindcursor) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"bind cursor...");
		#endif
		if (cursor->sql_injection_detection_ingress(
						cursor->querybuffer)) {
			success=true;
		} else {
			success=cursor->executeQuery(cursor->querybuffer,
							cursor->querylength,
							reallyexecute);
		}
		cursor->sid_egress_success=
			cursor->sql_injection_detection_egress();
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"preparing/executing...");
		#endif
		if (supportsNativeBinds()) {
			if (cursor->sql_injection_detection_ingress(
							cursor->querybuffer)) {
				success=true;
			} else {
				success=(cursor->prepareQuery(
							cursor->querybuffer,
							cursor->querylength) && 
					cursor->handleBinds() && 
					cursor->executeQuery(
							cursor->querybuffer,
							cursor->querylength,
							true));
			}
			cursor->sid_egress_success=
				cursor->sql_injection_detection_egress();
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
				success=true;
			} else {
				success=(cursor->prepareQuery(
							cursor->querybuffer,
							cursor->querylength) && 
					cursor->executeQuery(queryptr,
							querylen,true));
			}
			cursor->sid_egress_success=
				cursor->sql_injection_detection_egress();
			delete newquery;
		}
	}

	if (!cursor->sid_egress_success) {
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
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"commit necessary...");
		#endif
		success=commit();
		commitorrollback=false;
	}

	#ifdef SERVER_DEBUG
	if (success) {
		debugPrint("connection",2,"processing query succeeded");
	} else {
		debugPrint("connection",2,"processing query failed");
	}
	debugPrint("connection",2,"done processing query");
	#endif

	return success;
}

void sqlrconnection_svr::commitOrRollback(sqlrcursor_svr *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"commit or rollback check...");
	#endif

	// if the query was a commit or rollback, set a flag indicating so
	if (isTransactional()) {
		if (cursor->queryIsCommitOrRollback()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback not needed");
			#endif
			commitorrollback=false;
		} else if (cursor->queryIsNotSelect()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback needed");
			#endif
			commitorrollback=true;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done with commit or rollback check");
	#endif
}
