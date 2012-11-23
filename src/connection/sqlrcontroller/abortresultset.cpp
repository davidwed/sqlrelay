// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

void sqlrcontroller_svr::abortResultSetCommand(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",1,"aborting result set...");

	// Very important...
	// Do not cleanUpData() here, otherwise result sets that were suspended
	// after the entire result set was fetched won't be able to return
	// column data when resumed.
	cursor->abort();

	dbgfile.debugPrint("connection",1,"done aborting result set");
}
