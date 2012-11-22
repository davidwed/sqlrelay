// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::returnResultSetData(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning result set data...");

	// get the number of rows to skip
	uint64_t	skip;
	if (clientsock->read(&skip,idleclienttimeout,0)!=sizeof(uint64_t)) {
		dbgfile.debugPrint("connection",2,
				"returning result set data failed");
		return false;
	}

	// get the number of rows to fetch
	uint64_t	fetch;
	if (clientsock->read(&fetch,idleclienttimeout,0)!=sizeof(uint64_t)) {
		dbgfile.debugPrint("connection",2,
				"returning result set data failed");
		return false;
	}

	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((uint16_t)END_RESULT_SET);
		flushWriteBuffer();
		dbgfile.debugPrint("connection",2,
				"done returning result set data");
		return true;
	}


	// reinit suspendresultset
	cursor->suspendresultset=false;


	// skip the specified number of rows
	if (!skipRows(cursor,skip)) {
		clientsock->write((uint16_t)END_RESULT_SET);
		flushWriteBuffer();
		dbgfile.debugPrint("connection",2,
				"done returning result set data");
		return true;
	}


	if (dbgfile.debugEnabled()) {
		debugstr=new stringbuffer();
		debugstr->append("fetching ");
		debugstr->append(fetch);
		debugstr->append(" rows...");
		dbgfile.debugPrint("connection",2,debugstr->getString());
		delete debugstr;
	}

	// send the specified number of rows back
	for (uint64_t i=0; (!fetch || i<fetch); i++) {

		if (!cursor->fetchRow()) {
			clientsock->write((uint16_t)END_RESULT_SET);
			flushWriteBuffer();
			dbgfile.debugPrint("connection",2,
					"done returning result set data");
			return true;
		}

		if (dbgfile.debugEnabled()) {
			debugstr=new stringbuffer();
		}

		cursor->returnRow();

		if (dbgfile.debugEnabled()) {
			dbgfile.debugPrint("connection",3,
						debugstr->getString());
			delete debugstr;
		}

		if (cursor->lastrowvalid) {
			cursor->lastrow++;
		} else {
			cursor->lastrowvalid=true;
			cursor->lastrow=0;
		}
	}
	flushWriteBuffer();

	dbgfile.debugPrint("connection",2,"done returning result set data");
	return true;
}
