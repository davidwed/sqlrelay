// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int	sqlrcursor::fetchFromBindCursor() {

	if (!endofresultset || !sqlrc->connected) {
		return 0;
	}

	// FIXME: should these be here?
	clearVariables();
	clearResultSet();

	cached=0;
	endofresultset=0;

	// tell the server we're fetching from a bind cursor
	sqlrc->write((unsigned short)FETCH_FROM_BIND_CURSOR);

	// send the cursor id to the server
	sqlrc->write((unsigned short)cursorid);

	sendGetColumnInfo();

	if (processResultSet(rsbuffersize-1)) {
		return 1;
	}
	return 0;
}
