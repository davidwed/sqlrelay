// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrcursor::abort() {
	// Very important...
	// Do not cleanUpData() here, otherwise result sets that were suspended
	// after the entire result set was fetched won't be able to return
	// column data when resumed.
	suspendresultset=false;
	busy=false;
}

void sqlrcursor::cleanUpData(bool freerows, bool freecols, bool freebinds) {
	// by default, do nothing...
	return;
}
