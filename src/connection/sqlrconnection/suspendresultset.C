// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::suspendResultSetCommand(sqlrcursor *cursor) {
	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"suspend result set");
	#endif
	cursor->suspendresultset=true;
}
