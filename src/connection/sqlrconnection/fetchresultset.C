// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::fetchResultSetCommand() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",1,"fetch result set");
	#endif
	if (!returnResultSetData()) {
		endSession();
		return 0;
	}
	return 1;
}
