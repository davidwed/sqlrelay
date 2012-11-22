// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <config.h>

#include <sqlrconnection.h>

const char *sqlrcursor_svr::getColumnName(uint32_t col) {
	return NULL;
}

uint16_t sqlrcursor_svr::getColumnNameLength(uint32_t col) {
	return charstring::length(getColumnName(col));
}

uint16_t sqlrcursor_svr::getColumnType(uint32_t col) {
	return UNKNOWN_DATATYPE;
}

const char *sqlrcursor_svr::getColumnTypeName(uint32_t col) {
	return NULL;
}

uint16_t sqlrcursor_svr::getColumnTypeNameLength(uint32_t col) {
	return charstring::length(getColumnTypeName(col));
}

uint32_t sqlrcursor_svr::getColumnLength(uint32_t col) {
	return 0;
}

uint32_t sqlrcursor_svr::getColumnPrecision(uint32_t col) {
	return 0;
}

uint32_t sqlrcursor_svr::getColumnScale(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsNullable(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsPrimaryKey(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsUnique(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsPartOfKey(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsUnsigned(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsZeroFilled(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsBinary(uint32_t col) {
	return 0;
}

uint16_t sqlrcursor_svr::getColumnIsAutoIncrement(uint32_t col) {
	return 0;
}

void sqlrcursor_svr::returnRow() {

	// run through the columns...
	for (uint32_t i=0; i<colCount(); i++) {

		// init variables
		const char	*field=NULL;
		uint64_t	fieldlength=0;
		bool		blob=false;
		bool		null=false;

		// get the field
		getField(i,&field,&fieldlength,&blob,&null);

		// send data to the client
		if (null) {
			conn->sendNullField();
		} else if (blob) {
			sendLobField(i);
			cleanUpLobField(i);
		} else {
			conn->sendField(field,fieldlength);
		}
	}

	// get the next row
	nextRow();
}

void sqlrcursor_svr::getField(uint32_t col,
				const char **field, uint64_t *fieldlength,
				bool *blob, bool *null) {
	// by default, do nothing
}

void sqlrcursor_svr::nextRow() {
	// by default, do nothing
}

#define MAX_BYTES_PER_CHAR	4

void sqlrcursor_svr::sendLobField(uint32_t col) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!getLobFieldLength(col,&loblength)) {
		conn->sendNullField();
		return;
	}

	// for lobs of 0 length
	if (!loblength) {
		conn->startSendingLong(0);
		conn->sendLongSegment("",0);
		conn->endSendingLong();
		return;
	}

	// initialize sizes and status
	uint64_t	charstoread=sizeof(lobbuffer)/MAX_BYTES_PER_CHAR;
	uint64_t	charsread=0;
	uint64_t	offset=0;
	bool		start=true;

	for (;;) {

		// read a segment from the lob
		if (!getLobFieldSegment(col,lobbuffer,sizeof(lobbuffer),
					offset,charstoread,&charsread) ||
					!charsread) {

			// if we fail to get a segment or got nothing...
			// if we haven't started sending yet, then send a NULL,
			// otherwise just end normally
			if (start) {
				conn->sendNullField();
			} else {
				conn->endSendingLong();
			}
			return;

		} else {

			// if we haven't started sending yet, then do that now
			if (start) {
				conn->startSendingLong(loblength);
				start=false;
			}

			// send the segment we just got
			conn->sendLongSegment(lobbuffer,charsread);

			// FIXME: or should this be charsread?
			offset=offset+charstoread;
		}
	}
}

bool sqlrcursor_svr::getLobFieldLength(uint32_t col, uint64_t *length) {
	*length=0;
	return true;
}

bool sqlrcursor_svr::getLobFieldSegment(uint32_t col,
					char *buffer, uint64_t buffersize,
					uint64_t offset, uint64_t charstoread,
					uint64_t *charsread) {
	*charsread=0;
	return false;
}

void sqlrcursor_svr::cleanUpLobField(uint32_t col) {
	// by default, do nothing
}
