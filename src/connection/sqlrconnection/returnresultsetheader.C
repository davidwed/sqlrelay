// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::returnResultSetHeader(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set header...");
	#endif


	// return the row counts
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"returning row counts...");
	#endif
	cursor->returnRowCounts();
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning row counts");
	#endif


	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write((unsigned short)sendcolumninfo);

	#ifdef SERVER_DEBUG
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		debugPrint("connection",3,"column info will be sent");
	} else {
		debugPrint("connection",3,"column info will not be sent");
	}
	#endif


	// return the column count
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"returning column counts...");
	#endif
	cursor->returnColumnCount();
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning column counts");
	#endif


	if (sendcolumninfo==SEND_COLUMN_INFO) {
		// return the column info
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"returning column info...");
		#endif
		cursor->returnColumnInfo();
		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"done returning column info");
		#endif
	}


	// return the output bind vars
	returnOutputBindValues(cursor);


	// terminate the bind vars
	clientsock->write((unsigned short)END_BIND_VARS);


	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set header");
	#endif
}
