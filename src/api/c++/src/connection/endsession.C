// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

void sqlrconnection::endSession() {

	if (debug) {
		debugPreStart();
		debugPrint("Ending Session\n");
		debugPreEnd();
	}

	// abort each cursor's result set
	sqlrcursor	*currentcursor=firstcursor;
	while (currentcursor) {
		if (!currentcursor->endofresultset) {
			currentcursor->abortResultSet();
		}
		currentcursor=currentcursor->next;
	}

	// write an END_SESSION to the connection
	if (connected) {
		cs->write((unsigned short)END_SESSION);
		flushWriteBuffer();
		endsessionsent=true;
		closeConnection();
	}
}
