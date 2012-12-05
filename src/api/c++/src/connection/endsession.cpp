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
			currentcursor->closeResultSet(false);
		}
		currentcursor->havecursorid=false;
		currentcursor=currentcursor->next;
	}

	// write an END_SESSION to the connection
	if (connected) {
		cs->write((uint16_t)END_SESSION);
		flushWriteBuffer();
		endsessionsent=true;
		closeConnection();
	}
}
