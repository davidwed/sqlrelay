// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::skipRows(sqlrcursor_svr *cursor, uint64_t rows) {

	debugstr=new stringbuffer();
	debugstr->append("skipping ");
	debugstr->append(rows);
	debugstr->append(" rows...");
	dbgfile.debugPrint("connection",2,debugstr->getString());
	delete debugstr;

	for (uint64_t i=0; i<rows; i++) {

		dbgfile.debugPrint("connection",3,"skip...");

		if (lastrowvalid) {
			lastrow++;
		} else {
			lastrowvalid=true;
			lastrow=0;
		}

		if (!cursor->skipRow()) {
			dbgfile.debugPrint("connection",2,
				"skipping rows hit the end of the result set");
			return false;
		}
	}

	dbgfile.debugPrint("connection",2,"done skipping rows");
	return true;
}
