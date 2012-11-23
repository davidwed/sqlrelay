// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::sendField(const char *data, uint32_t size) {

	if (dbgfile.debugEnabled()) {
		debugstr->append("\"");
		debugstr->append(data,size);
		debugstr->append("\",");
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection_svr::sendNullField() {

	if (dbgfile.debugEnabled()) {
		debugstr->append("NULL");
	}

	clientsock->write((uint16_t)NULL_DATA);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrconnection_svr::sendLobField(sqlrcursor_svr *cursor, uint32_t col) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cursor->getLobFieldLength(col,&loblength)) {
		sendNullField();
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		startSendingLong(0);
		sendLongSegment("",0);
		endSendingLong();
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(cursor->lobbuffer)/
						MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!cursor->getLobFieldSegment(col,
					cursor->lobbuffer,
					sizeof(cursor->lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				sendNullField();
			} else {
				endSendingLong();
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			sendLongSegment(cursor->lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

void sqlrconnection_svr::startSendingLong(uint64_t longlength) {
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrconnection_svr::sendLongSegment(const char *data, uint32_t size) {

	if (dbgfile.debugEnabled()) {
		debugstr->append(data,size);
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection_svr::endSendingLong() {

	if (dbgfile.debugEnabled()) {
		debugstr->append(",");
	}

	clientsock->write((uint16_t)END_LONG_DATA);
}
