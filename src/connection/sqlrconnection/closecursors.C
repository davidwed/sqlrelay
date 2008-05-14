// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::closeCursors(bool destroy) {

	dbgfile.debugPrint("connection",0,"closing cursors...");

	if (cur) {
		for (int32_t i=0; i<cfgfl->getCursors(); i++) {

			dbgfile.debugPrint("connection",1,i);

			if (cur[i]) {
				cur[i]->closeCursor();
				if (destroy) {
					deleteCursorUpdateStats(cur[i]);
				}
			}
		}
		if (destroy) {
			delete[] cur;
			cur=NULL;
		}
	}

	dbgfile.debugPrint("connection",0,"done closing cursors...");
}
