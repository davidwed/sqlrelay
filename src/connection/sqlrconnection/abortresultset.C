// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::abortResultSetCommand(sqlrcursor *cursor) {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"abort result set");
	#endif
	cursor->abort();
}
