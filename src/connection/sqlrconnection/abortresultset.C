// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::abortResultSetCommand(sqlrcursor_svr *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"abort result set");
	#endif

	// Very important...
	// Do not cleanUpData() here, otherwise result sets that were suspended
	// after the entire result set was fetched won't be able to return
	// column data when resumed.
	cursor->abort();
}
