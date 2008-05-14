// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <datatypes.h>

void sqlrconnection_svr::returnResultSetHeader(sqlrcursor_svr *cursor) {

	// if sid egress check failed, return 0 rows and columns
	if (cursor->sid_egress) {
		dbgfile.debugPrint("connection",2,
				"sid egress check failed...");
		dbgfile.debugPrint("connection",2,
				"returning empty result set header...");
		// row counts
		sendRowCounts(cursor->knowsRowCount(),0,
				cursor->knowsAffectedRows(),0);
		// send column info or not
		clientsock->write((uint16_t)DONT_SEND_COLUMN_INFO);
		// column count
		clientsock->write((uint32_t)0);
		// no bind vars
		clientsock->write((uint16_t)END_BIND_VARS);
		dbgfile.debugPrint("connection",2,
				"done returning result set header");
		return;
	}

	dbgfile.debugPrint("connection",2,"returning result set header...");

	// return the row counts
	dbgfile.debugPrint("connection",3,"returning row counts...");
	sendRowCounts(cursor->knowsRowCount(),cursor->rowCount(),
			cursor->knowsAffectedRows(),cursor->affectedRows());
	dbgfile.debugPrint("connection",3,"done returning row counts");


	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write(sendcolumninfo);

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		dbgfile.debugPrint("connection",3,"column info will be sent");
	} else {
		dbgfile.debugPrint("connection",3,"column info will not be sent");
	}


	// return the column count
	dbgfile.debugPrint("connection",3,"returning column counts...");
	clientsock->write(cursor->colCount());
	dbgfile.debugPrint("connection",3,"done returning column counts");


	if (sendcolumninfo==SEND_COLUMN_INFO) {

		// return the column type format
		dbgfile.debugPrint("connection",2,"sending column type format...");
		uint16_t	format=cursor->columnTypeFormat();
		if (format==COLUMN_TYPE_IDS) {
			dbgfile.debugPrint("connection",3,"id's");
		} else {
			dbgfile.debugPrint("connection",3,"names");
		}
		clientsock->write(format);
		dbgfile.debugPrint("connection",2,"done sending column type format");

		// return the column info
		dbgfile.debugPrint("connection",3,"returning column info...");
		cursor->returnColumnInfo();
		dbgfile.debugPrint("connection",3,"done returning column info");
	}


	// return the output bind vars
	returnOutputBindValues(cursor);


	// terminate the bind vars
	clientsock->write((uint16_t)END_BIND_VARS);

	flushWriteBuffer();

	dbgfile.debugPrint("connection",2,"done returning result set header");
}
