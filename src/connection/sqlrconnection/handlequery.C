// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int sqlrconnection::handleQuery(sqlrcursor *cursor,
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
			clientsock->write((unsigned short)NO_ERROR);

			// send the client the id of the 
			// cursor that it's going to use
			clientsock->write((unsigned short)cursor->id);

			// tell the client that this is not a
			// suspended result set
			clientsock->write((unsigned short)
						NO_SUSPENDED_RESULT_SET);

			// if the query processed 
			// ok then return a result set
			// header and loop back to send the
			// result set itself...
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

				// client will be sending skip/fetch,
				// better get it even though we're not gonna
				// use it
				unsigned long	skipfetch;
				clientsock->read(&skipfetch);
				clientsock->read(&skipfetch);

				// Even though there was an error, we still 
				// need to send the client the id of the 
				// cursor that it's going to use.
				clientsock->write((unsigned short)cursor->id);

				#ifdef SERVER_DEBUG
				debugPrint("connection",1,
					"failed to handle query: error");
				#endif
				return -1;
			}
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done handling query");
	#endif
	return 1;
}

bool sqlrconnection::getQueryFromClient(sqlrcursor *cursor,
					bool reexecute, bool bindcursor) {

	// if we're not reexecuting and not using a bound cursor, get the query,
	// if we're not using a bound cursor, get the input/output binds,
	// get whether to send column info or not
	return (((reexecute || bindcursor)?true:getQuery(cursor)) &&
		((bindcursor)?true:
		(getInputBinds(cursor) && getOutputBinds(cursor))) &&
		getSendColumnInfo());
}

bool sqlrconnection::getQuery(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting query...");
	#endif

	// get the length of the query
	if (clientsock->read(&cursor->querylength)!=
					sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query length size");
		#endif
		return false;
	}

	// bounds checking
	if (cursor->querylength>MAXQUERYSIZE) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		#endif
		return false;
	}

	// read the query into the buffer
	if ((unsigned long)(clientsock->read(cursor->querybuffer,
						cursor->querylength))!=
					(unsigned long)(cursor->querylength)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent short query");
		#endif
		return false;
	}
	cursor->querybuffer[cursor->querylength]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"querylength:");
	debugPrint("connection",4,(long)cursor->querylength);
	debugPrint("connection",3,"query:");
	debugPrint("connection",0,cursor->querybuffer);
	debugPrint("connection",2,"getting query succeeded");
	#endif

	return true;
}

bool sqlrconnection::processQuery(sqlrcursor *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute) {

	// Very important...
	// Clean up data here instead of when aborting a result set, this
	// allows for result sets that were suspended after the entire
	// result set was fetched to still be able to return column data
	// when resumed.
	if (bindcursor) {
		cursor->cleanUpData(false,false,true);
	} else {
		cursor->cleanUpData(true,true,true);
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
		success=(cursor->handleBinds() && 
			cursor->executeQuery(cursor->querybuffer,
						cursor->querylength,
						reallyexecute));
	} else if (bindcursor) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"bind cursor...");
		#endif
		success=cursor->executeQuery(cursor->querybuffer,
						cursor->querylength,
						reallyexecute);
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"preparing/executing...");
		#endif
		success=(cursor->prepareQuery(cursor->querybuffer,
						cursor->querylength) && 
			cursor->handleBinds() && 
			cursor->executeQuery(cursor->querybuffer,
						cursor->querylength,true));
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

void sqlrconnection::commitOrRollback(sqlrcursor *cursor) {

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
