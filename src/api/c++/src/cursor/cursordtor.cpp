// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>

sqlrcursor::~sqlrcursor() {

	// abort result set if necessary
	if (sqlrc && !sqlrc->endsessionsent && !sqlrc->suspendsessionsent) {
		closeResultSet(true);
	}

	// deallocate copied references
	deleteVariables();

	// deallocate the query buffer
	delete[] querybuffer;

	// deallocate the fullpath (used for file queries)
	delete[] fullpath;

	clearResultSet();
	delete[] columns;
	delete[] extracolumns;
	delete colstorage;
	if (rows) {
		for (uint32_t i=0; i<OPTIMISTIC_ROW_COUNT; i++) {
			delete rows[i];
		}
		delete[] rows;
	}
	delete rowstorage;

	// it's possible for the connection to be deleted before the 
	// cursor is, in that case, don't do any of this stuff
	if (sqlrc) {

		// remove self from connection's cursor list
		if (!next && !prev) {
			sqlrc->firstcursor=NULL;
			sqlrc->lastcursor=NULL;
		} else {
			sqlrcursor	*temp=next;
			if (next) {
				next->prev=prev;
			} else {
				sqlrc->lastcursor=prev;
			}
			if (prev) {
				prev->next=temp;
			} else {
				sqlrc->firstcursor=next;
			}
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Deallocated cursor\n");
			sqlrc->debugPreEnd();
		}
	}

	if (copyrefs && cachedestname) {
		delete[] cachedestname;
	}
	delete[] cachedestindname;
}
