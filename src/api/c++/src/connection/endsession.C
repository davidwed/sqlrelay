// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int	sqlrconnection::endSession() {

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

	// write a ~ to the connection
	int	retval=1;
	if (connected) {
		retval=0;
		write((unsigned short)END_SESSION);
		endsessionsent=1;
		retval=1;
		closeConnection();
	}
	return retval;
}
