// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

void sqlrcursor::setResultSetBufferSize(uint32_t rows) {
	rsbuffersize=rows;
	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Result Set Buffer Size: ");
		sqlrc->debugPrint((int32_t)rows);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}
}

uint32_t sqlrcursor::getResultSetBufferSize() {
	return rsbuffersize;
}
