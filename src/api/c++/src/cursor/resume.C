// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

int sqlrcursor::resumeResultSet(int id) {
	return resumeCachedResultSet(id,NULL);
}

int sqlrcursor::resumeCachedResultSet(int id, const char *filename) {

	if (!endofresultset && !suspendresultsetsent) {
		abortResultSet();
	}
	clearResultSet();

	if (!sqlrc->connected) {
		return 0;
	}

	cached=false;
	resumed=true;
	endofresultset=false;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		sqlrc->debugPrint((long)id);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// tell the server we want to resume the result set
	sqlrc->write((unsigned short)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	sqlrc->write((unsigned short)id);

	// process the result set
	if (filename && filename[0]) {
		cacheToFile(filename);
	}
	if (rsbuffersize) {
		if (processResultSet(firstrowindex+rsbuffersize-1)) {
			return 1;
		}
	} else {
		if (processResultSet(-1)) {
			return 1;
		}
	}
	return 0;
}
