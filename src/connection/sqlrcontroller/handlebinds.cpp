// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

bool sqlrcontroller_svr::handleBinds(sqlrcursor_svr *cursor) {

	bindvar_svr	*bind=NULL;
	
	// iterate through the arrays, binding values to variables
	for (int16_t i=0; i<cursor->inbindcount; i++) {

		bind=&cursor->inbindvars[i];

		// bind the value to the variable
		if (bind->type==STRING_BIND || bind->type==NULL_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==INTEGER_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval)) {
				return false;
			}
		} else if (bind->type==DOUBLE_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					bind->value.doubleval.precision,
					bind->value.doubleval.scale)) {
				return false;
			}
		} else if (bind->type==DATE_BIND) {
			if (!cursor->inputBind(
					bind->variable,
					bind->variablesize,
					bind->value.dateval.year,
					bind->value.dateval.month,
					bind->value.dateval.day,
					bind->value.dateval.hour,
					bind->value.dateval.minute,
					bind->value.dateval.second,
					bind->value.dateval.microsecond,
					bind->value.dateval.tz,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==BLOB_BIND) {
			if (!cursor->inputBindBlob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CLOB_BIND) {
			if (!cursor->inputBindClob(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize,
					&bind->isnull)) {
				return false;
			}
		}
	}

	for (int16_t i=0; i<cursor->outbindcount; i++) {

		bind=&cursor->outbindvars[i];

		// bind the value to the variable
		if (bind->type==STRING_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					bind->value.stringval,
					bind->valuesize+1,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==INTEGER_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.integerval,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==DOUBLE_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.doubleval.value,
					&bind->value.doubleval.precision,
					&bind->value.doubleval.scale,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==DATE_BIND) {
			if (!cursor->outputBind(
					bind->variable,
					bind->variablesize,
					&bind->value.dateval.year,
					&bind->value.dateval.month,
					&bind->value.dateval.day,
					&bind->value.dateval.hour,
					&bind->value.dateval.minute,
					&bind->value.dateval.second,
					&bind->value.dateval.microsecond,
					(const char **)&bind->value.dateval.tz,
					bind->value.dateval.buffer,
					bind->value.dateval.buffersize,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==BLOB_BIND) {
			if (!cursor->outputBindBlob(
					bind->variable,
					bind->variablesize,i,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CLOB_BIND) {
			if (!cursor->outputBindClob(
					bind->variable,
					bind->variablesize,i,
					&bind->isnull)) {
				return false;
			}
		} else if (bind->type==CURSOR_BIND) {

			bool	found=false;

			// find the cursor that we acquird earlier...
			for (uint16_t j=0; j<cursorcount; j++) {

				if (cur[j]->id==bind->value.cursorid) {
					found=true;

					// bind the cursor
					if (!cursor->outputBindCursor(
							bind->variable,
							bind->variablesize,
							cur[j])) {
						return false;
					}
					break;
				}
			}

			// this shouldn't happen, but if it does, return false
			if (!found) {
				return false;
			}
		}
	}
	return true;
}

void sqlrcontroller_svr::returnOutputBindBlob(sqlrcursor_svr *cursor,
							uint16_t index) {
	return sendLobOutputBind(cursor,index);
}

void sqlrcontroller_svr::returnOutputBindClob(sqlrcursor_svr *cursor,
							uint16_t index) {
	return sendLobOutputBind(cursor,index);
}

#define MAX_BYTES_PER_CHAR	4

void sqlrcontroller_svr::sendLobOutputBind(sqlrcursor_svr *cursor,
							uint16_t index) {

	// Get lob length.  If this fails, send a NULL field.
	uint64_t	loblength;
	if (!cursor->getLobOutputBindLength(index,&loblength)) {
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
		if (!cursor->getLobOutputBindSegment(
					index,
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
