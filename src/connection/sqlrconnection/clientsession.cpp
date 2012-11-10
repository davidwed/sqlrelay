// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

// for gettimeofday()
#include <sys/time.h>

void sqlrconnection_svr::clientSession() {

	dbgfile.debugPrint("connection",0,"client session...");

	incrementClientSessionCount();

	// During each session, the client will send a series of commands.
	// The session ends when the client ends it or when certain commands
	// fail.
	bool	loop=true;
	do {

		// get a command from the client
		uint16_t	command;
		if (!getCommand(&command)) {
			endSessionCommand();
			break;
		}

		// get the command start time
		timeval		tv;
		struct timezone	tz;
		gettimeofday(&tv,&tz);

		// these commands are all handled at the connection level
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
		} else if (command==BEGIN) {
			beginCommand();
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

		// For the rest of the commands,
		// the client will request a cursor
		sqlrcursor_svr	*cursor=getCursor(command);
		if (!cursor) {
			noAvailableCursors(command);
			continue;
		}

		// keep track of the command start time
		cursor->commandstartsec=tv.tv_sec;
		cursor->commandstartusec=tv.tv_usec;

		// these commands are all handled at the cursor level
		if (command==NEW_QUERY) {
			loop=newQueryCommand(cursor);
		} else if (command==REEXECUTE_QUERY) {
			loop=reExecuteQueryCommand(cursor);
		} else if (command==FETCH_FROM_BIND_CURSOR) {
			loop=fetchFromBindCursorCommand(cursor);
		} else if (command==FETCH_RESULT_SET) {
			loop=fetchResultSetCommand(cursor);
		} else if (command==ABORT_RESULT_SET) {
			abortResultSetCommand(cursor);
		} else if (command==SUSPEND_RESULT_SET) {
			suspendResultSetCommand(cursor);
		} else if (command==RESUME_RESULT_SET) {
			loop=resumeResultSetCommand(cursor);
		} else if (command==GETDBLIST) {
			loop=getDatabaseListCommand(cursor);
		} else if (command==GETTABLELIST) {
			loop=getTableListCommand(cursor);
		} else if (command==GETCOLUMNLIST) {
			loop=getColumnListCommand(cursor);
		} else {
			endSessionCommand();
			loop=false;
		}

		// get the command end-time
		gettimeofday(&tv,&tz);
		cursor->commandendsec=tv.tv_sec;
		cursor->commandendusec=tv.tv_usec;

		// log query-related commands
		// FIXME: should we really log bind cursor fetches?
		// FIXME: this won't log triggers
		if (sqlrlg &&
			(command==NEW_QUERY ||
			command==REEXECUTE_QUERY ||
			command==FETCH_FROM_BIND_CURSOR)) {
			sqlrlg->runLoggers(this,cursor);
		}

	} while (loop);

	waitForClientClose();

	closeSuspendedSessionSockets();

	decrementClientSessionCount();

	dbgfile.debugPrint("connection",0,"done with client session");
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
				"client cursor request failed, "
				"cursor id stage");
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

	// find an available cursor
	for (uint16_t i=0; i<cursorcount; i++) {
		if (!cur[i]->busy) {
			dbgfile.debugPrint("connection",2,"available cursor:");
			dbgfile.debugPrint("connection",3,(int32_t)i);
			return cur[i];
		}
	}

	// apparently there weren't any available cursors...

	// if we can't create any new cursors then return an error
	if (cursorcount==maxcursorcount) {
		dbgfile.debugPrint("connection",2,"all cursors are busy");
		return NULL;
	}

	// create new cursors
	uint16_t	expandto=cursorcount+cfgfl->getCursorsGrowBy();
	if (expandto>=maxcursorcount) {
		expandto=maxcursorcount;
	}
	uint16_t	firstnewcursor=cursorcount;
	do {
		cur[cursorcount]=initCursorInternal();
		cur[cursorcount]->suspendresultset=false;
		if (!cur[cursorcount]->openCursorInternal(cursorcount)) {
			dbgfile.debugPrint("connection",1,
					"cursor init failure...");
			logOutInternal();
			return NULL;
		}
		cursorcount++;
	} while (cursorcount<expandto);
	
	// return the first new cursor that we created
	return cur[firstnewcursor];
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
	clientsock->write((uint16_t)ERROR_OCCURRED);

	// send the error code (zero for now)
	clientsock->write((uint64_t)0);

	// send the error itself
	uint16_t	len=charstring::length(NOCURSORSERROR);
	clientsock->write(len);
	clientsock->write(NOCURSORSERROR,len);
	flushWriteBuffer();
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
			"closing sockets from a previously "
			"suspended session...");
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
			"done closing sockets from a previously "
			"suspended session...");
	}
}
