// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

char	*sqlrconnection::identify() {

	if (!openSession()) {
		return 0;
	}

	if (debug) {
		debugPreStart();
		debugPrint("Identifying...");
		debugPrint("\n");
		debugPreEnd();
	}

	write((unsigned short)IDENTIFY);

	// get the id
	unsigned short	size;
	if (read(&size)==sizeof(unsigned short)) {
		id=new char[size+1];
		if (read(id,size)!=size) {
			setError("Failed to identify.\n A network error may have ocurred.");
			delete[] id;
			return NULL;
		}
		id[size]=(char)NULL;

		if (debug) {
			debugPreStart();
			debugPrint(id);
			debugPrint("\n");
			debugPreEnd();
		}
	} else {
		setError("Failed to identify.\n A network error may have ocurred.");
		return NULL;
	}
	return id;
}
