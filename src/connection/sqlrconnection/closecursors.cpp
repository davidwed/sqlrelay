// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::closeCursors(bool destroy) {

	dbgfile.debugPrint("connection",0,"closing cursors...");

	if (cur) {
		while (cursorcount) {
			cursorcount--;

			dbgfile.debugPrint("connection",1,(int32_t)cursorcount);

			if (cur[cursorcount]) {
				cur[cursorcount]->cleanUpData(true,true);
				cur[cursorcount]->closeCursor();
				if (destroy) {
					deleteCursorUpdateStats(
							cur[cursorcount]);
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
