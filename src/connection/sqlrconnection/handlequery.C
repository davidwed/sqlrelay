// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::handleQuery(int reexecute, int bindcursor,
							int reallyexecute) {


	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"handling query...");
	#endif

	if (!getQueryFromClient(reexecute,bindcursor)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,"failed to handle query");
		#endif
		return 0;
	}

	// loop here to handle down databases
	for (;;) {

		// process the query
		if (processQuery(reexecute,bindcursor,reallyexecute)) {

			// indicate that no error has occurred
			clientsock->write((unsigned short)NO_ERROR);

			// send the client the id of the 
			// cursor that it's going to use
			clientsock->write((unsigned short)currentcur);

			// tell the client that this is not a
			// suspended result set
			clientsock->write(
				(unsigned short)NO_SUSPENDED_RESULT_SET);

			// if the query processed 
			// ok then return a result set
			// header and loop back to send the
			// result set itself...
			returnResultSetHeader();

			// free memory used by binds
			bindpool->free();

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,"handle query succeeded");
			#endif
			return 1;

		} else {

			// If the query didn't process ok,
			// handle the error.
			// If handleError returns a 0 then the error 
			// was a down database that has presumably
			// come back up by now.  Loop back...
			if (handleError()) {

				// client will be sending skip/fetch,
				// better get it even though we're not gonna
				// use it
				unsigned long	skipfetch;
				clientsock->read(&skipfetch);
				clientsock->read(&skipfetch);

				cur[currentcur]->abort();
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

int	sqlrconnection::getQueryFromClient(int reexecute, int bindcursor) {

	// if we're not reexecuting and not using a bound cursor, get the query,
	// if we're not using a bound cursor, get the input/output binds,
	// get whether to send column info or not
	return (((reexecute || bindcursor)?1:getQuery()) &&
		((bindcursor)?1:(getInputBinds() && getOutputBinds())) &&
		getSendColumnInfo());
}

int	sqlrconnection::getQuery() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting query...");
	#endif

	// get the length of the query
	if (clientsock->read(&cur[currentcur]->querylength)!=
					sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query length size");
		#endif
		return 0;
	}

	// bounds checking
	if (cur[currentcur]->querylength>MAXQUERYSIZE) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent bad query size");
		#endif
		return 0;
	}

	// read the query into the buffer
	if ((unsigned long)(clientsock->read(cur[currentcur]->querybuffer,
				cur[currentcur]->querylength))!=
					(unsigned long)(cur[currentcur]->
								querylength)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting query failed: client sent short query");
		#endif
		return 0;
	}
	cur[currentcur]->querybuffer[cur[currentcur]->querylength]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"querylength:");
	debugPrint("connection",4,(long)cur[currentcur]->querylength);
	debugPrint("connection",3,"query:");
	debugPrint("connection",0,cur[currentcur]->querybuffer);
	debugPrint("connection",2,"getting query succeeded");
	#endif

	return 1;
}

int	sqlrconnection::processQuery(int reexecute, int bindcursor,
							int reallyexecute) {

	// Very important...
	// Clean up data here instead of when aborting a result set, this
	// allows for result sets that were suspended after the entire
	// result set was fetched to still be able to return column data
	// when resumed.
	if (bindcursor) {
		cur[currentcur]->cleanUpData(false,false,true);
	} else {
		cur[currentcur]->cleanUpData(true,true,true);
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"processing query...");
	#endif

	// if the reexecute flag is set, the query doesn't need to be prepared 
	// again.
	int	success=0;
	if (reexecute) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"re-executing...");
		#endif
		success=cur[currentcur]->handleBinds() && 
			cur[currentcur]->executeQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength,
					reallyexecute);
	} else if (bindcursor) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"bind cursor...");
		#endif
		success=cur[currentcur]->executeQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength,
					reallyexecute);
	} else {
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"preparing/executing...");
		#endif
		success=cur[currentcur]->prepareQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength) && 
			cur[currentcur]->handleBinds() && 
			cur[currentcur]->executeQuery(
					cur[currentcur]->querybuffer,
					cur[currentcur]->querylength,1);
	}

	// was the query a commit or rollback?
	commitOrRollback();

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
		commitorrollback=0;
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

void	sqlrconnection::commitOrRollback() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"commit or rollback check...");
	#endif

	// if the query was a commit or rollback, set a flag indicating so
	if (isTransactional()) {
		if (cur[currentcur]->queryIsCommitOrRollback()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback not needed");
			#endif
			commitorrollback=0;
		} else if (cur[currentcur]->queryIsNotSelect()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",3,
					"commit or rollback needed");
			#endif
			commitorrollback=1;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done with commit or rollback check");
	#endif
}
