// Copyright (c) 2007  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrconnection::fakeBinds() {

	if (!openSession()) {
		return false;
	}

	if (debug) {
		debugPreStart();
		debugPrint("fake binds...");
		debugPrint("\n");
		debugPreEnd();
	}

	cs->write((uint16_t)FAKEBINDS);
	flushWriteBuffer();

	// get the fakebinds
	bool	fakebinds;
	if (cs->read(&fakebinds)!=sizeof(bool)) {
		setError("Failed to get fake binds status.\n A network error may have ocurred.");
		return false;
	}

	if (debug) {
		debugPreStart();
		debugPrint((int64_t)fakebinds);
		debugPrint("\n");
		debugPreEnd();
	}
	return fakebinds;
}
