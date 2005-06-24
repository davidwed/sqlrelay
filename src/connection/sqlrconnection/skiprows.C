// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::skipRows(sqlrcursor *cursor, uint64_t rows) {

	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	debugstr->append("skipping ");
	debugstr->append(rows);
	debugstr->append(" rows...");
	debugPrint("connection",2,debugstr->getString());
	delete debugstr;
	#endif

	for (uint64_t i=0; i<rows; i++) {

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,"skip...");
		#endif

		if (lastrowvalid) {
			lastrow++;
		} else {
			lastrowvalid=true;
			lastrow=0;
		}

		if (!cursor->skipRow()) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"skipping rows hit the end of the result set");
			#endif
			return false;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done skipping rows");
	#endif
	return true;
}
