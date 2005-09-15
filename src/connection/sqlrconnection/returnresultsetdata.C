// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::returnResultSetData(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set data...");
	#endif

	// get the number of rows to skip
	uint64_t	skip;
	if (clientsock->read(&skip,idleclienttimeout,0)!=sizeof(uint64_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return false;
	}

	// get the number of rows to fetch
	uint64_t	fetch;
	if (clientsock->read(&fetch,idleclienttimeout,0)!=sizeof(uint64_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return false;
	}


	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((uint16_t)END_RESULT_SET);
		flushWriteBuffer();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return true;
	}


	// reinit suspendresultset
	cursor->suspendresultset=false;


	// skip the specified number of rows
	if (!skipRows(cursor,skip)) {
		clientsock->write((uint16_t)END_RESULT_SET);
		flushWriteBuffer();
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return true;
	}


	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	debugstr->append("fetching ");
	debugstr->append(fetch);
	debugstr->append(" rows...");
	debugPrint("connection",2,debugstr->getString());
	delete debugstr;
	#endif

	// send the specified number of rows back
	for (uint64_t i=0; (!fetch || i<fetch); i++) {

		if (!cursor->fetchRow()) {
			clientsock->write((uint16_t)END_RESULT_SET);
			flushWriteBuffer();
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"done returning result set data");
			#endif
			return true;
		}

		#ifdef SERVER_DEBUG
		debugstr=new stringbuffer();
		#endif
		cursor->returnRow();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,debugstr->getString());
		delete debugstr;
		#endif

		if (lastrowvalid) {
			lastrow++;
		} else {
			lastrowvalid=true;
			lastrow=0;
		}
	}
	flushWriteBuffer();

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set data");
	#endif
	return true;
}
