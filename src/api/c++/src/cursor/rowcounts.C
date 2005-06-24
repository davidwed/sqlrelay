// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

uint64_t sqlrcursor::firstRowIndex() {
	return firstrowindex;
}

bool sqlrcursor::endOfResultSet() {
	return endofresultset;
}

uint64_t sqlrcursor::rowCount() {
	return rowcount;
}

uint64_t sqlrcursor::affectedRows() {
	if (knowsaffectedrows==AFFECTED_ROWS) {
		return affectedrows;
	}
	return 0;
}

uint64_t sqlrcursor::totalRows() {
	if (knowsactualrows==ACTUAL_ROWS) {
		return actualrows;
	}
	return 0;
}
