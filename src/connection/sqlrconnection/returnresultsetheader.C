// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <datatypes.h>

void sqlrconnection_svr::returnResultSetHeader(sqlrcursor_svr *cursor) {

	// if sid egress check failed, return 0 rows and columns
	if (!cursor->sid_egress_success) {
printf("egress check failed\n");
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"sid egress check failed...");
		debugPrint("connection",2,
				"returning empty result set header...");
		#endif
		// row counts
		sendRowCounts(cursor->knowsRowCount(),0,
				cursor->knowsAffectedRows(),0);
		// send column info or not
		clientsock->write(DONT_SEND_COLUMN_INFO);
		// column count
		clientsock->write(0);
		// no bind vars
		clientsock->write((uint16_t)END_BIND_VARS);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"done returning result set header");
		#endif
		return;
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning result set header...");
	#endif

	// return the row counts
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"returning row counts...");
	#endif
	sendRowCounts(cursor->knowsRowCount(),cursor->rowCount(),
			cursor->knowsAffectedRows(),cursor->affectedRows());
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning row counts");
	#endif


	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write(sendcolumninfo);

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
	clientsock->write(cursor->colCount());
	#ifdef SERVER_DEBUG
	debugPrint("connection",3,"done returning column counts");
	#endif


	if (sendcolumninfo==SEND_COLUMN_INFO) {

		// return the column type format
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"sending column type format...");
		#endif
		uint16_t	format=cursor->columnTypeFormat();
		#ifdef SERVER_DEBUG
		if (format==COLUMN_TYPE_IDS) {
			debugPrint("connection",3,"id's");
		} else {
			debugPrint("connection",3,"names");
		}
		#endif
		clientsock->write(format);
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"done sending column type format");
		#endif

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
	clientsock->write((uint16_t)END_BIND_VARS);

	flushWriteBuffer();

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning result set header");
	#endif
}
