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
				cur[cursorcount]->close();
				if (destroy) {
					deleteCursorInternal(
						cur[cursorcount]);
					cur[cursorcount]=NULL;
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

void sqlrconnection_svr::deleteCursorInternal(sqlrcursor_svr *curs) {
	deleteCursor(curs);
	semset->waitWithUndo(9);
	statistics->open_svr_cursors--;
	if (statistics->open_svr_cursors<0) {
		statistics->open_svr_cursors=0;
	}
	semset->signalWithUndo(9);
}
