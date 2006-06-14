// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::clientSession() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"client session...");
	#endif

	statistics->open_cli_connections++;
	statistics->opened_cli_connections++;

	// a session consists of getting a query and returning a result set
	// over and over
	for (;;) {

		// is this a query, fetch, abort, suspend or resume...
		uint16_t	command;
		if (!getCommand(&command)) {
			endSession();
			break;
		}

		// handle some things up front
		if (command==AUTHENTICATE) {
			if (authenticateCommand()) {
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
			endSession();
			break;
		}
	}

	waitForClientClose();

	closeSuspendedSessionSockets();

	statistics->open_cli_connections--;

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done with client session");
	#endif
}

sqlrcursor_svr *sqlrconnection_svr::getCursor(uint16_t command) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"getting a cursor...");
	#endif

	// does the client need a cursor or does it already have one
	uint16_t	neednewcursor=DONT_NEED_NEW_CURSOR;
	if (command==NEW_QUERY &&
		clientsock->read(&neednewcursor,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"client cursor request failed, need new cursor stage");
		#endif
		return NULL;
	}

	sqlrcursor_svr	*cursor=NULL;

	if (neednewcursor==DONT_NEED_NEW_CURSOR) {

		// which cursor is the client requesting?
		uint16_t	index;
		if (clientsock->read(&index,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"client cursor request failed, cursor index stage");
			#endif
			return NULL;
		}

		// don't allow the client to request a cursor 
		// beyond the end of the array.
		if (index>cfgfl->getCursors()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"client requested an invalid cursor:");
			debugPrint("connection",3,(int32_t)index);
			#endif
			return NULL;
		}

		statistics->times_cursor_reused++;

		// set the current cursor to the one they requested.
		cursor=cur[index];

	} else {

		statistics->times_cursor_reused++;

		// find an available cursor
		cursor=findAvailableCursor();
	}

	if (cursor) {
		cursor->busy=true;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done getting a cursor");
	#endif
	return cursor;
}

sqlrcursor_svr *sqlrconnection_svr::findAvailableCursor() {

	for (int16_t i=0; i<cfgfl->getCursors(); i++) {
		if (!cur[i]->busy) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,"found a free cursor:");
			debugPrint("connection",3,(int32_t)i);
			#endif
			return cur[i];
		}
	}
	#ifdef SERVER_DEBUG
	debugPrint("connection",2,
			"find available cursor failed: all cursors are busy");
	#endif
	return NULL;
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
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"waiting for client to close the connection...");
	#endif
	uint16_t	dummy;
	clientsock->read(&dummy,idleclienttimeout,0);
	clientsock->close();
	delete clientsock;
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,
			"done waiting for client to close the connection...");
	#endif
}

void sqlrconnection_svr::closeSuspendedSessionSockets() {

	// If we're no longer in a suspended session and we we're passing 
	// around file descriptors but had to open a set of sockets to handle 
	// a suspended session, close those sockets here.
	if (!suspendedsession && cfgfl->getPassDescriptor()) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"closing sockets from a previously suspended session...");
		#endif
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
		}
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
			"done closing sockets from a previously suspended session...");
		#endif
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
	clientsock->write((uint16_t)62);
	clientsock->write("No server-side cursors were available to process the query.",62);
	flushWriteBuffer();
}

bool sqlrconnection_svr::getCommand(uint16_t *command) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"getting command...");
	#endif

	// get the command
	if (clientsock->read(command,idleclienttimeout,0)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",1,
		"getting command failed: client sent bad command or timed out");
		#endif
		return false;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"done getting command");
	#endif
	return true;
}
