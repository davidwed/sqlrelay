// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

void	sqlrcursor::setResultSetBufferSize(int rows) {
	rsbuffersize=rows;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Result Set Buffer Size: ");
		sqlrc->debugPrint((long)rows);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
}

int	sqlrcursor::getResultSetBufferSize() {
	return rsbuffersize;
}
