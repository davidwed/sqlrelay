// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::skipRows(int rows) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"skipping rows...");
	#endif

	for (int i=0; i<rows; i++) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"skip...");
		#endif

		lastrow++;
		if (!cur[currentcur]->skipRow()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"skipping rows hit the end of the result set");
			#endif
			return 0;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done skipping rows");
	#endif
	return 1;
}
