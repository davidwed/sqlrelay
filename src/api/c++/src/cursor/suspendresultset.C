// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void sqlrcursor::suspendResultSet() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Suspending Result Set\n");
		sqlrc->debugPreEnd();
	}
	if (sqlrc->connected && !cached) {
		sqlrc->write((unsigned short)SUSPEND_RESULT_SET);
		sqlrc->write(cursorid);
	}
	clearCacheDest();
	suspendresultsetsent=1;
}
