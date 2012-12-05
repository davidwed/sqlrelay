// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrcursor::resumeResultSet(uint16_t id) {
	return resumeCachedResultSet(id,NULL);
}

bool sqlrcursor::resumeCachedResultSet(uint16_t id, const char *filename) {

	if (!endofresultset && !suspendresultsetsent) {
		closeResultSet(false);
	}
	clearResultSet();

	if (!sqlrc->connected) {
		return false;
	}

	cached=false;
	resumed=true;
	endofresultset=false;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Resuming Result Set of Cursor: ");
		sqlrc->debugPrint((int64_t)id);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// tell the server we want to resume the result set
	sqlrc->cs->write((uint16_t)RESUME_RESULT_SET);

	// send the id of the cursor we want to 
	// resume the result set of to the server
	sqlrc->cs->write(id);

	// process the result set
	if (filename && filename[0]) {
		cacheToFile(filename);
	}
	if (rsbuffersize) {
		if (processResultSet(true,firstrowindex+rsbuffersize-1)) {
			return true;
		}
	} else {
		if (processResultSet(false,0)) {
			return true;
		}
	}
	return false;
}
