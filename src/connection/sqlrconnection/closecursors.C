// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::closeCursors(bool destroy) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"closing cursors...");
	#endif

	if (cur) {
		for (int32_t i=0; i<cfgfl->getCursors(); i++) {

			#ifdef SERVER_DEBUG
			debugPrint("connection",1,i);
			#endif

			if (cur[i]) {
				cur[i]->closeCursor();
				if (destroy) {
					deleteCursor(cur[i]);
				}
			}
		}
		if (destroy) {
			delete[] cur;
			cur=NULL;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",0,"done closing cursors...");
	#endif
}
