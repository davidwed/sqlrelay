// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrcursor::fetchFromBindCursor() {

	if (!endofresultset || !sqlrc->connected) {
		return false;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	cached=false;
	endofresultset=false;

	// tell the server we're fetching from a bind cursor
	sqlrc->cs->write((unsigned short)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	sqlrc->cs->write((unsigned short)cursorid);

	sendGetColumnInfo();

	sqlrc->flushWriteBuffer();

	return processResultSet(rsbuffersize-1);
}
