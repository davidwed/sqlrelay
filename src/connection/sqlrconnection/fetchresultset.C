// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::fetchResultSetCommand(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"fetch result set");
	#endif
	if (!returnResultSetData(cursor)) {
		endSession();
		return false;
	}
	return true;
}
