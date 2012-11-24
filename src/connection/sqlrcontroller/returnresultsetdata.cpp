// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::returnResultSetData(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning result set data...");

	updateState(RETURN_RESULT_SET);

	// decide whether to use the cursor itself
	// or an attached custom query cursor
	if (cursor->customquerycursor) {
		cursor=cursor->customquerycursor;
	}

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

	// reinit cursor state (in case it was suspended)
	cursor->state=SQLRCURSORSTATE_BUSY;

	// for some queries, there are no rows to return, 
	if (cursor->noRowsToReturn()) {
		clientsock->write((uint16_t)END_RESULT_SET);
		flushWriteBuffer();
		dbgfile.debugPrint("connection",2,
				"done returning result set data");
		return true;
	}

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

		returnRow(cursor);

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

bool sqlrcontroller_svr::skipRows(sqlrcursor_svr *cursor, uint64_t rows) {

	if (dbgfile.debugEnabled()) {
		debugstr=new stringbuffer();
		debugstr->append("skipping ");
		debugstr->append(rows);
		debugstr->append(" rows...");
		dbgfile.debugPrint("connection",2,debugstr->getString());
		delete debugstr;
	}

	for (uint64_t i=0; i<rows; i++) {

		dbgfile.debugPrint("connection",3,"skip...");

		if (cursor->lastrowvalid) {
			cursor->lastrow++;
		} else {
			cursor->lastrowvalid=true;
			cursor->lastrow=0;
		}

		if (!cursor->skipRow()) {
			dbgfile.debugPrint("connection",2,
				"skipping rows hit the end of the result set");
			return false;
		}
	}

	dbgfile.debugPrint("connection",2,"done skipping rows");
	return true;
}

void sqlrcontroller_svr::returnRow(sqlrcursor_svr *cursor) {

	// run through the columns...
	for (uint32_t i=0; i<cursor->colCount(); i++) {

		// init variables
		const char	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;

		// get the field
		cursor->getField(i,&field,&fieldlength,&blob,&null);

		// send data to the client
		if (null) {
			sendNullField();
		} else if (blob) {
			sendLobField(cursor,i);
			cursor->cleanUpLobField(i);
		} else {
			sendField(field,fieldlength);
		}
	}

	// get the next row
	cursor->nextRow();
}
