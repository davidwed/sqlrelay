// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int	sqlrcursor::firstRowIndex() {
	return firstrowindex;
}

int	sqlrcursor::endOfResultSet() {
	return endofresultset;
}

int	sqlrcursor::rowCount() {
	return rowcount;
}

int	sqlrcursor::affectedRows() {
	if (knowsaffectedrows==AFFECTED_ROWS) {
		return affectedrows;
	}
	return -1;
}

int	sqlrcursor::totalRows() {
	if (knowsactualrows==ACTUAL_ROWS) {
		return actualrows;
	}
	return -1;
}
