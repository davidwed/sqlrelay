// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sys/types.h>
#include <unistd.h>

void sqlrconnection_svr::incrementClientSessionCount() {

	dbgfile.debugPrint("connection",0,"incrementing client session count...");

	if (inclientsession) {
		dbgfile.debugPrint("connection",0,"error. already in client session");
		return;
	}

	semset->waitWithUndo(9);
	statistics->open_cli_connections++;
	inclientsession=true;
	statistics->opened_cli_connections++;
	semset->signalWithUndo(9);

	dbgfile.debugPrint("connection",0,"done incrementing client session count...");
}

void sqlrconnection_svr::decrementClientSessionCount() {

	dbgfile.debugPrint("connection",0,"decrementing client session count...");

	if (!inclientsession) {
		dbgfile.debugPrint("connection",0,"error. not in client session");
		return;
	}

	semset->waitWithUndo(9);
	statistics->open_cli_connections--;
	inclientsession=false;
	if (statistics->open_cli_connections<0) {
		statistics->open_cli_connections=0;
	}
	semset->signalWithUndo(9);

	dbgfile.debugPrint("connection",0,"done decrementing client session count...");
}

void sqlrconnection_svr::clientSession() {

	dbgfile.debugPrint("connection",0,"client session...");

	incrementClientSessionCount();

	// a session consists of getting a query and returning a result set
	// over and over
	for (;;) {

		// is this a query, fetch, abort, suspend or resume...
		uint16_t	command;
		if (!getCommand(&command)) {
			endSessionCommand();
			break;
		}

		// handle some things up front
		if (command==AUTHENTICATE) {
			if (authenticateCommand()) {
				sessionStartQueries();
				continue;
			}
			break;
		} else if (command==SUSPEND_SESSION) {
			suspendSessionCommand();
			break;
		} else if (command==END_SESSION) {
			endSessionCommand();
			break;
		} else if (command==PING) {
			pingCommand();
			continue;
		} else if (command==IDENTIFY) {
			identifyCommand();
			continue;
		} else if (command==AUTOCOMMIT) {
			autoCommitCommand();
			continue;
		} else if (command==COMMIT) {
			commitCommand();
			continue;
		} else if (command==ROLLBACK) {
			rollbackCommand();
			continue;
		} else if (command==DBVERSION) {
			dbVersionCommand();
			continue;
		} else if (command==BINDFORMAT) {
			bindFormatCommand();
			continue;
		} else if (command==SERVERVERSION) {
			serverVersionCommand();
			continue;
		} else if (command==SELECT_DATABASE) {
			selectDatabaseCommand();
			continue;
		} else if (command==GET_CURRENT_DATABASE) {
			getCurrentDatabaseCommand();
			continue;
		} else if (command==GET_LAST_INSERT_ID) {
			getLastInsertIdCommand();
			continue;
		}

		// For the rest of the commands, the client will be requesting
		// a cursor.  Get the cursor to work with.  Save the result of
		// this, the client may be sending more information and we need
		// to collect it.
		sqlrcursor_svr	*cursor=getCursor(command);
		if (!cursor) {
			noAvailableCursors(command);
			continue;
		}

		if (command==NEW_QUERY) {
			if (newQueryCommand(cursor)) {
				continue;
			}
			break;
		} else if (command==GETDBLIST) {
			if (getDatabaseListCommand(cursor)) {
				continue;
			}
			break;
		} else if (command==GETTABLELIST) {
			if (getTableListCommand(cursor)) {
				continue;
			}
			break;
		} else if (command==GETCOLUMNLIST) {
			if (getColumnListCommand(cursor)) {
				continue;
			}
			break;
		} else if (command==REEXECUTE_QUERY) {
			if (!reExecuteQueryCommand(cursor)) {
				break;
			}
		} else if (command==FETCH_FROM_BIND_CURSOR) {
			if (!fetchFromBindCursorCommand(cursor)) {
				break;
			}
		} else if (command==FETCH_RESULT_SET) {
			if (!fetchResultSetCommand(cursor)) {
				break;
			}
		} else if (command==ABORT_RESULT_SET) {
			abortResultSetCommand(cursor);
		} else if (command==SUSPEND_RESULT_SET) {
			suspendResultSetCommand(cursor);
		} else if (command==RESUME_RESULT_SET) {
			if (!resumeResultSetCommand(cursor)) {
				break;
			}
		} else {
			endSessionCommand();
			break;
		}
	}

	waitForClientClose();

	closeSuspendedSessionSockets();

	decrementClientSessionCount();

	dbgfile.debugPrint("connection",0,"done with client session");
}

sqlrcursor_svr *sqlrconnection_svr::getCursor(uint16_t command) {

	dbgfile.debugPrint("connection",1,"getting a cursor...");

	// does the client need a cursor or does it already have one
	uint16_t	neednewcursor=DONT_NEED_NEW_CURSOR;
	if ((command==NEW_QUERY ||
		command==GETDBLIST ||
		command==GETTABLELIST ||
		command==GETCOLUMNLIST ||
		command==ABORT_RESULT_SET) &&
		clientsock->read(&neednewcursor,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
			"client cursor request failed, need new cursor stage");
		return NULL;
	}

	sqlrcursor_svr	*cursor=NULL;

	if (neednewcursor==DONT_NEED_NEW_CURSOR) {

		// which cursor is the client requesting?
		uint16_t	id;
		if (clientsock->read(&id,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
			dbgfile.debugPrint("connection",2,
				"client cursor request failed, cursor id stage");
			return NULL;
		}

		// set the current cursor to the one they requested.
		bool	found=false;
		for (uint16_t i=0; i<cursorcount; i++) {
			if (cur[i]->id==id) {
				cursor=cur[i];

				semset->waitWithUndo(9);
				statistics->times_cursor_reused++;
				semset->signalWithUndo(9);

				found=true;
				break;
			}
		}

		// don't allow the client to request a cursor 
		// beyond the end of the array.
		if (!found) {
			dbgfile.debugPrint("connection",2,
				"client requested an invalid cursor:");
			dbgfile.debugPrint("connection",3,(int32_t)id);
			return NULL;
		}

	} else {

		semset->waitWithUndo(9);
 		
 		// mark this as a new cursor being used
		statistics->times_new_cursor_used++;
		
		semset->signalWithUndo(9);

		// find an available cursor
		cursor=findAvailableCursor();
	}

	if (cursor) {
		cursor->busy=true;
	}

	dbgfile.debugPrint("connection",1,"done getting a cursor");
	return cursor;
}

sqlrcursor_svr *sqlrconnection_svr::findAvailableCursor() {

	uint16_t	i=0;
	for (; i<cursorcount; i++) {
		if (!cur[i]->busy) {
			dbgfile.debugPrint("connection",2,"found a free cursor:");
			dbgfile.debugPrint("connection",3,(int32_t)i);
			return cur[i];
		}
	}

	// check if dynamic cursors is disabled, spit out the v0.41 error
	if (cfgfl->getMaxCursors()==0) {
		dbgfile.debugPrint("connection",2,
			"find available cursor failed: all cursors are busy");
		return NULL;
	}

	// if we're already at a maximum amount of cursors, return failure
	uint16_t	expandto=cursorcount+cfgfl->getCursorsGrowBy();
	if (expandto>=cfgfl->getMaxCursors()) {
		dbgfile.debugPrint("connection", 1, "Can't expand cursors, already at maximum");
		return NULL;
	}

	// we're here because there are no free cursors left.
	// Lets make some more baby!
	sqlrcursor_svr	**tmp =
			(sqlrcursor_svr **)realloc((void *)cur,
					sizeof(sqlrcursor_svr **)*expandto);
	
	// in case we're out of memory, we don't want to destroy the cur array
	if (tmp==NULL) {
		dbgfile.debugPrint("connection",1,
				"Out of memory allocating cursors");
		return NULL;
	}
	cursorcount=expandto;
	cur=tmp;
	
	uint16_t	firstopen=i;
	
	for (uint16_t j=firstopen; j<cursorcount; j++,i++) {
		cur[i]=initCursorUpdateStats();
		// FIXME: LAME!!!  oh god this is lame....
		cur[i]->querybuffer=new char[maxquerysize+1];
		cur[i]->suspendresultset=false;
		if (!cur[i]->openCursorInternal(i)) {
			dbgfile.debugPrint("connection",1,"realloc cursor init failure...");

			logOutUpdateStats();
			return NULL;
		}
	}
	
	return cur[firstopen];
}

void sqlrconnection_svr::waitForClientClose() {

	// Sometimes the server sends the result set and closes the socket
	// while part of it is buffered but not yet transmitted.  This caused
	// the client to receive a partial result set or error.  Telling the
	// socket to linger doesn't always fix it.  Doing a read here should 
	// guarantee that the client will close it's end of the connection 
	// before the server closes it's end; the server will wait for data 
	// from the client (which it will never receive) and when the client 
	// closes it's end (which it will only do after receiving the entire
	// result set) the read will fall through.  This should guarantee 
	// that the client will get the the entire result set without
	// requiring the client to send data back indicating so.
	dbgfile.debugPrint("connection",1,
			"waiting for client to close the connection...");
	uint16_t	dummy;
	clientsock->read(&dummy,idleclienttimeout,0);
	clientsock->close();
	delete clientsock;
	dbgfile.debugPrint("connection",1,
			"done waiting for client to close the connection...");
}

void sqlrconnection_svr::closeSuspendedSessionSockets() {

	// If we're no longer in a suspended session and we we're passing 
	// around file descriptors but had to open a set of sockets to handle 
	// a suspended session, close those sockets here.
	if (!suspendedsession && cfgfl->getPassDescriptor()) {
		dbgfile.debugPrint("connection",1,
			"closing sockets from a previously suspended session...");
		if (serversockun) {
			removeFileDescriptor(serversockun);
			delete serversockun;
			serversockun=NULL;
		}
		if (serversockin) {
			for (uint64_t index=0;
					index<serversockincount;
					index++) {
				removeFileDescriptor(serversockin[index]);
				delete serversockin[index];
				serversockin[index]=NULL;
			}
			delete[] serversockin;
			serversockin=NULL;
			serversockincount=0;
		}
		dbgfile.debugPrint("connection",1,
			"done closing sockets from a previously suspended session...");
	}
}

void sqlrconnection_svr::noAvailableCursors(uint16_t command) {

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
				MAXVAR*(2*sizeof(uint16_t)+BINDVARLENGTH)+
				// output bind var count
				sizeof(uint16_t)+
				// output bind vars
				MAXVAR*(2*sizeof(uint16_t)+BINDVARLENGTH)+
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
	clientsock->write((uint16_t)ERROR);

	// send the error itself
	uint16_t	len=charstring::length(NOCURSORSERROR);
	clientsock->write(len);
	clientsock->write(NOCURSORSERROR,len);
	flushWriteBuffer();
}

bool sqlrconnection_svr::getCommand(uint16_t *command) {

	dbgfile.debugPrint("connection",1,"getting command...");

	// get the command
	if (clientsock->read(command,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",1,
		"getting command failed: client sent bad command or timed out");
		return false;
	}

	dbgfile.debugPrint("connection",1,"done getting command");
	return true;
}

void sqlrconnection_svr::sessionStartQueries() {
	// run a configurable set of queries at the start of each session
	for (stringlistnode *node=
		cfgfl->getSessionStartQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getData());
	}
}

void sqlrconnection_svr::sessionEndQueries() {
	// run a configurable set of queries at the end of each session
	for (stringlistnode *node=
		cfgfl->getSessionEndQueries()->getFirstNode();
						node; node=node->getNext()) {
		sessionQuery(node->getData());
	}
}

void sqlrconnection_svr::sessionQuery(const char *query) {

	// create the select database query
	size_t	querylen=charstring::length(query);

	sqlrcursor_svr	*cur=initCursorUpdateStats();

	// since we're creating a new cursor for this, make sure it
	// can't have an ID that might already exist
	if (cur->openCursorInternal(cursorcount+1) &&
		cur->prepareQuery(query,querylen) &&
		executeQueryUpdateStats(cur,query,querylen,true)) {
		cur->cleanUpData(true,true);
	}
	cur->closeCursor();
	deleteCursorUpdateStats(cur);
}
