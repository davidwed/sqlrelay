// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::returnResultSetData(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set data...");
	#endif

	// get the number of rows to skip
	unsigned long	skip;
	if (clientsock->read(&skip)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return false;
	}

	// get the number of rows to fetch
	unsigned long	fetch;
	if (clientsock->read(&fetch)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"returning result set data failed");
		#endif
		return false;
	}


	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((unsigned short)END_RESULT_SET);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return true;
	}


	// reinit suspendresultset
	cursor->suspendresultset=false;


	// skip the specified number of rows
	if (!skipRows(cursor,skip)) {
		clientsock->write((unsigned short)END_RESULT_SET);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done returning result set data");
		#endif
		return true;
	}


	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	debugstr->append("fetching ");
	debugstr->append((long)fetch);
	debugstr->append(" rows...");
	debugPrint("connection",2,debugstr->getString());
	delete debugstr;
	#endif

	// send the specified number of rows back
	for (unsigned long i=0; (!fetch || i<fetch); i++) {

		if (!cursor->fetchRow()) {
			clientsock->write((unsigned short)END_RESULT_SET);
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

		lastrow++;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set data");
	#endif
	return true;
}
