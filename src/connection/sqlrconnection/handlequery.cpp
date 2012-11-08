// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

// for gettimeofday()
#include <sys/time.h>

bool sqlrconnection_svr::handleQuery(sqlrcursor_svr *cursor,
					bool reexecute, bool bindcursor,
					bool reallyexecute, bool getquery) {


	dbgfile.debugPrint("connection",1,"handling query...");

	// clear bind mappings and reset fakeinputbindsforthisquery flag
	if (!reexecute && !bindcursor) {
		bindmappingspool->free();
		inbindmappings->clear();
		outbindmappings->clear();
		cursor->fakeinputbindsforthisquery=fakeinputbinds;
	}

	// get the query and bind data from the client
	if (getquery) {
		bool	success=true;
		if (!reexecute && !bindcursor) {
			success=getQuery(cursor);
		}
		if (success && !bindcursor) {
			success=(getInputBinds(cursor) &&
					getOutputBinds(cursor));
		}
		if (success) {
			success=getSendColumnInfo();
		}
		if (!success) {
			dbgfile.debugPrint("connection",1,
						"failed to handle query");
			endSession();
			return false;
		}
	}

	// loop here to handle down databases
	const char	*error;
	int64_t		errnum;
	bool		liveconnection;
	for (;;) {

		// init error
		error=NULL;
		errnum=0;
		liveconnection=true;

		// process the query
		bool	success=false;
		bool	wasfaketransactionquery=false;
		if (!reexecute && !bindcursor && faketransactionblocks) {
			success=handleFakeTransactionQueries(cursor,
						&wasfaketransactionquery,
						&error,
						&errnum);
		}
		if (!wasfaketransactionquery) {
			success=processQuery(cursor,reexecute,
						bindcursor,reallyexecute);
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

			// handle after-triggers
			if (sqltr) {
				sqltr->runAfterTriggers(this,cursor,
							cursor->querytree,true);
			}

			// reinit lastrow
			lastrowvalid=false;

			// return the result set
			if (!returnResultSetData(cursor)) {
				endSession();
				return false;
			}
			return true;

		} else {

			// get the error message from the database
			// (unless it was already set)
			if (!error) {
				cursor->errorMessage(&error,&errnum,
							&liveconnection);
			}

			// if the db is still up, or if we're not waiting
			// for them if they're down, then return the error
			if (liveconnection ||
				!cfgfl->getWaitForDownDatabase()) {

				// return the error
				returnQueryError(cursor,error,
						errnum,!liveconnection);
			}

			// if the error was a dead connection
			// then re-establish the connection
			if (!liveconnection) {

				dbgfile.debugPrint("connection",3,
							"database is down...");
				reLogIn();

				// if we're waiting for down databases then
				// loop back and try the query again
				if (cfgfl->getWaitForDownDatabase()) {
					continue;
				}
			}

			// handle after-triggers
			if (sqltr) {
				sqltr->runAfterTriggers(
					this,cursor,
					cursor->querytree,false);
			}

			return true;
		}
	}
}

bool sqlrconnection_svr::getQuery(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"getting query...");

	// get the length of the client info
	// FIXME: arguably this should be it's own command
	if (clientsock->read(&clientinfolen)!=sizeof(uint64_t) ||
					clientinfolen>sizeof(clientinfo)-1) {
		dbgfile.debugPrint("connection",2,
			"getting client info failed: "
			"client sent bad client info size");
		return false;
	}
	if (clientsock->read(clientinfo,clientinfolen)!=clientinfolen) {
		dbgfile.debugPrint("connection",2,
			"getting client info failed: "
			"client sent short client info");
		return false;
	}
	clientinfo[clientinfolen]='\0';

	dbgfile.debugPrint("connection",3,"clientinfolen:");
	dbgfile.debugPrint("connection",4,(int32_t)clientinfolen);
	dbgfile.debugPrint("connection",3,"clientinfo:");
	dbgfile.debugPrint("connection",0,clientinfo);
	dbgfile.debugPrint("connection",2,"getting clientinfo succeeded");

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

bool sqlrconnection_svr::getSendColumnInfo() {

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

	if (reexecute &&
		!cursor->fakeinputbindsforthisquery &&
		cursor->supportsNativeBinds()) {

		// if we're reexecuting and not faking binds then
		// the statement doesn't need to be prepared again...

		// handle before-triggers
		if (sqltr) {
			sqltr->runBeforeTriggers(this,cursor,
						cursor->querytree);
		}

		dbgfile.debugPrint("connection",3,"re-executing...");
		success=(cursor->handleBinds() && 
			executeQueryInternal(cursor,
						cursor->querybuffer,
						cursor->querylength,
						reallyexecute));

	} else if (bindcursor) {

		// if the cursor is a bind cursor then we just need to
		// execute, we don't need to worry about binds...

		dbgfile.debugPrint("connection",3,"bind cursor...");
		// FIXME: should we be passing
		// in the querybuffer and length here?
		success=executeQueryInternal(cursor,
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

			// handle before-triggers
			if (sqltr) {
				sqltr->runBeforeTriggers(this,cursor,
							cursor->querytree);
			}

			const char	*queryptr=cursor->querybuffer;
			uint32_t	querylen=cursor->querylength;

			// fake input binds if necessary
			stringbuffer	*newquery=NULL;
			if (cursor->fakeinputbindsforthisquery ||
				!cursor->supportsNativeBinds()) {

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
			if (success &&
				!cursor->fakeinputbindsforthisquery &&
				cursor->supportsNativeBinds()) {
				success=cursor->handleBinds();
			}

			// execute
			if (success) {
				success=executeQueryInternal(
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
	if (success && isTransactional() && commitorrollback &&
					fakeautocommit && autocommit) {
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

bool sqlrconnection_svr::executeQueryInternal(sqlrcursor_svr *curs,
							const char *query,
							uint32_t length,
							bool execute) {

	// update query count
	semset->waitWithUndo(9);
	statistics->total_queries++;
	semset->signalWithUndo(9);

	// variables for query timing
	timeval		starttv;
	struct timezone	starttz;
	timeval		endtv;
	struct timezone	endtz;

	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {
		// get the query start time
		gettimeofday(&starttv,&starttz);
	}

	// execute the query
	bool	result=curs->executeQuery(query,length,execute);

	if (cfgfl->getTimeQueriesSeconds()>-1 &&
		cfgfl->getTimeQueriesMicroSeconds()>-1) {

		// get the query end time
		gettimeofday(&endtv,&endtz);

		// update stats
		curs->stats.query=query;
		curs->stats.result=result;
		curs->stats.sec=endtv.tv_sec-starttv.tv_sec;
		curs->stats.usec=endtv.tv_usec-starttv.tv_usec;

		// write out a log entry
		if (curs->stats.sec>=
				(uint64_t)cfgfl->getTimeQueriesSeconds() &&
			curs->stats.usec>=
				(uint64_t)cfgfl->getTimeQueriesMicroSeconds()) {
			writeQueryLog(curs);
		}
	}

	// update error count
	if (!result) {
		semset->waitWithUndo(9);
		statistics->total_errors++;
		semset->signalWithUndo(9);
		return false;
	}
	return true;
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

void sqlrconnection_svr::setFakeInputBinds(bool fake) {
	fakeinputbinds=fake;
}
